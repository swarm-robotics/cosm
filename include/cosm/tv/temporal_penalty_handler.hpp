/**
 * \file temporal_penalty_handler.hpp
 *
 * \copyright 2018 John Harwell, All rights reserved.
 *
 * This file is part of COSM.
 *
 * COSM is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * COSM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * COSM.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef INCLUDE_COSM_TV_TEMPORAL_PENALTY_HANDLER_HPP_
#define INCLUDE_COSM_TV_TEMPORAL_PENALTY_HANDLER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <algorithm>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "rcppsw/control/config/waveform_config.hpp"
#include "rcppsw/control/periodic_waveform.hpp"
#include "rcppsw/control/waveform_generator.hpp"
#include "rcppsw/er/client.hpp"
#include "rcppsw/multithread/lockable.hpp"

#include "cosm/tv/temporal_penalty.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, tv);

/*******************************************************************************
 * Classes
 ******************************************************************************/

/**
 * \class temporal_penalty_handler
 * \ingroup tv
 *
 * \brief The penalty handler for penalties for robots (e.g. how long they have
 * to wait when they pickup/drop a block).
 *
 * Does not do much more than provide the penalty list, and functions for
 * manipulating it to derived classes.
 */
class temporal_penalty_handler : public rer::client<temporal_penalty_handler>,
                                 public rmultithread::lockable {
 public:
  using const_iterator_type =
      typename std::list<temporal_penalty>::const_iterator;

  /**
   * \brief Initialize the penalty handler.
   *
   * \param config Parameters for penalty waveform generation.
   * \param name The name of the handler, for differentiating handler instancces
   * in logging statements.
   */
  temporal_penalty_handler(const rct::config::waveform_config* const config,
                           const std::string& name)
      : ER_CLIENT_INIT("cosm.tv.temporal_penalty_handler"),
        mc_name(name),
        m_waveform(rct::waveform_generator()(config->type, config)) {}

  ~temporal_penalty_handler(void) override = default;

  /* Not copy assignable/copy constructible by default */
  temporal_penalty_handler& operator=(const temporal_penalty_handler&) = delete;
  temporal_penalty_handler(const temporal_penalty_handler&) = delete;

#if (LIBRA_ER == LIBRA_ER_ALL)
  /**
   * \brief Get the name of the penalty handler (for debugging)
   */
  const std::string& name(void) const { return mc_name; }
#endif

  /**
   * \brief Get the next penalty which will be satisfied from the list.
   */
  temporal_penalty penalty_next(void) const {
    std::scoped_lock lock(m_list_mtx);
    return m_penalty_list.front();
  }

  /**
   * \brief Remove the specified penalty from the list once the robot it
   * corresponds to has served its penalty.
   *
   * \param victim The penalty to remove.
   * \param lock Is locking required around penalty list modifications or not?
   *             Should *ALWAYS* be \c TRUE if the function is called external
   *             to this class.
   */
  void penalty_remove(const temporal_penalty& victim, bool lock = true) {
    maybe_lock_wr(&m_list_mtx, lock);
    m_penalty_list.remove(victim);
    maybe_unlock_wr(&m_list_mtx, lock);
  }

  /**
   * \brief Abort a robot's serving of its penalty.
   *
   * This should only be done if the robot aborts its task WHILE also serving a
   * penalty.
   *
   * \param controller The robot to abort the penalty for.
   */
  void penalty_abort(const controller::base_controller& controller);

  /**
   * \brief Find the penalty object currently associated with the given
   * controller, if it exists (i.e. if the robot is currently serving a penalty)
   *
   * \param controller The controller to check.
   * \param lock Is locking required around penalty list modifications or not?
   *             Should *ALWAYS* be \c TRUE if the function is called external
   *             to this class.
   *
   * \return Iterator pointing to the penalty, or end() if none was found.
   */
  const_iterator_type penalty_find(const controller::base_controller& controller,
                                   bool lock = true) const {
    maybe_lock_rd(&m_list_mtx, lock);
    auto it = std::find_if(m_penalty_list.begin(),
                           m_penalty_list.end(),
                           [&](const temporal_penalty& p) {
                             return p.controller() == &controller;
                           });
    maybe_unlock_rd(&m_list_mtx, lock);
    return it;
  }
  /**
   * \brief If \c TRUE, then the specified robot is currently serving a cache
   * penalty.
   *
   * \param controller The controller to check penalty serving for.
   * \param lock Is locking required around penalty list modifications or not?
   *             Should *ALWAYS* be \c TRUE if the function is called external
   *             to this class.
   */
  RCPPSW_PURE bool
  is_serving_penalty(const controller::base_controller& controller,
                     bool lock = true) const {
    maybe_lock_rd(&m_list_mtx, lock);
    auto it = penalty_find(controller, false);
    bool ret = m_penalty_list.end() != it;
    maybe_unlock_rd(&m_list_mtx, lock);
    return ret;
  }

  /**
   * \brief Determine if a robot has satisfied the \ref temporal_penalty
   * it is currently serving yet.
   *
   * \param controller The robot to check. If the robot is not currently serving
   *                   a penalty, \c FALSE will be returned.
   *
   * \param timestep The current timestep.
   *
   * \return \c TRUE If the robot is currently waiting AND it has satisfied its
   * penalty.
   */
  RCPPSW_PURE bool
  is_penalty_satisfied(const controller::base_controller& controller,
                       const rtypes::timestep& t) const {
    std::scoped_lock lock(m_list_mtx);
    auto it = penalty_find(controller, false);
    if (it != m_penalty_list.end()) {
      return it->penalty_satisfied(t);
    }
    return false;
  }

  /**
   * \brief Calculate the penalty for a robot to serve for the operation, given
   * the current timestep and the configured penalty waveform.
   *
   * If the penalty for the robot was zero, we still need to make the robot
   * serve a 1 timestep penalty. Not needed for block ops (but doesn't really
   * hurt), but IS needed for cache ops, so that if two robots that enter a
   * cache on the same timestep and will serve 0 duration penalties things are
   * still handled properly. You can't rely on just checking the list in that
   * case, because 0 duration penalties are marked as served and removed from
   * the list the SAME timestep they are added, so the handler incorrectly
   * thinks that there is no conflict.
   */
  rtypes::timestep penalty_calc(const rtypes::timestep& t) const {
    rtypes::timestep penalty(0);

    /* can be NULL if penalty handling is disabled */
    if (nullptr != m_waveform) {
      penalty.set(static_cast<uint>(m_waveform->value(t.v())));
    }
    return penalty += static_cast<uint>(penalty == 0);
  }

 protected:
  template <typename TController>
  rtypes::timestep penalty_add(const TController* controller,
                               const rtypes::type_uuid& id,
                               const rtypes::timestep& orig_duration,
                               const rtypes::timestep& start) {
    /*
     * Note that the uniqueify AND actual list add operations must be covered by
     * the SAME lock-unlock sequence (not two separate sequences) in order for
     * all robots to always obey cache pickup policies. See COSM#625.
     */
    std::scoped_lock lock(m_list_mtx);
    auto duration = penalty_finish_uniqueify(start, orig_duration);
    m_penalty_list.push_back(temporal_penalty(controller, id, duration, start));
    return duration;
  }

 private:
  /*
   * \brief Deconflict penalties such that at most 1 robot finishes
   * serving their penalty per block/cache operation per timestep.
   *
   * \todo At some point this might need to be removed and concurrent penalty
   * finishes supported if it becomes a bottleneck (or if it seems to be overly
   * impacting performance from an avg # blocks collected/timestep
   * perspective).
   *
   * No locking is needed because this is a private function.
   *
   * \param duration The calculated penalty sans deconfliction. Passed by value
   *                 and modified, in order to make calculations simpler.
   */
  rtypes::timestep penalty_finish_uniqueify(const rtypes::timestep& start,
                                            rtypes::timestep duration) const {
    for (auto it = m_penalty_list.begin(); it != m_penalty_list.end(); ++it) {
      if (it->start_time() + it->penalty() == start + duration) {
        duration += 1;
        it = m_penalty_list.begin();
      }
    } /* for(i..) */
    return duration;
  }

  /* clang-format off */
  const std::string                   mc_name;

  std::list<temporal_penalty>         m_penalty_list{};
  mutable std::shared_mutex           m_list_mtx{};
  std::unique_ptr<rct::base_waveform> m_waveform;
  /* clang-format on */
};

NS_END(tv, cosm);

#endif /* INCLUDE_COSM_TV_TEMPORAL_PENALTY_HANDLER_HPP_ */
