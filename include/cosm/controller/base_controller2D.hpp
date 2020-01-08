/**
 * \file base_controller2D.hpp
 *
 * \copyright 2019 John Harwell, All rights reserved.
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

#ifndef INCLUDE_COSM_CONTROLLER_BASE_CONTROLLER2D_HPP_
#define INCLUDE_COSM_CONTROLLER_BASE_CONTROLLER2D_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <ext/ticpp/ticpp.h>

#include <memory>
#include <string>
#include <typeindex>

#include "rcppsw/er/client.hpp"
#include "rcppsw/math/rng.hpp"
#include "rcppsw/math/vector2.hpp"
#include "rcppsw/types/timestep.hpp"
#include "rcppsw/types/type_uuid.hpp"

#include "cosm/fsm/metrics/goal_acq_metrics.hpp"
#include "cosm/fsm/metrics/movement_metrics.hpp"
#include "cosm/metrics/spatial_dist2D_metrics.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm);

namespace tv {
class irv_manager;
} // namespace tv

namespace subsystem {
class saa_subsystem2D;
}

NS_START(controller);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class base_controller2D
 * \ingroup controller
 *
 * \brief The implementation of base controller class that the controllers for
 * all robots that operate in 2D derive. It holds all functionality
 * common to all controllers, as well that some that is stubbed out here, but
 * overridden in derived classes which allows this class to be used as the robot
 * controller handle when rendering QT graphics overlays.
 *
 * It should never be derived from directly.
 */
class base_controller2D : public cfmetrics::movement_metrics,
                          public cfmetrics::goal_acq_metrics,
                          public cmetrics::spatial_dist2D_metrics,
                          public rer::client<base_controller2D> {
 public:
  base_controller2D(void) RCSW_COLD;
  ~base_controller2D(void) override RCSW_COLD;

  base_controller2D(const base_controller2D&) = delete;
  base_controller2D& operator=(const base_controller2D&) = delete;

  /**
   * \brief Initialize the controller from XML configuration.
   */
  virtual void init(ticpp::Element&) RCSW_COLD = 0;

  /**
   * \brief Resut the controller before initialization, after simulation
   * finishes, etc. Should be idempotent, making starting running the controller
   * after calling this function the same as if the controller had just been
   * created.
   */
  virtual void reset(void) RCSW_COLD = 0;

  /**
   * \brief Run the main controller loop.
   */
  virtual void control_step(void) = 0;

  /**
   * \brief Return the \ref std::type_index of the derived class. This is useful
   * in conjunction with \ref boost::variant and \ref boost::apply_visitor, as
   * it allows for run-time reflection based on the actual type of the
   * controller.
   */
  virtual std::type_index type_index(void) const = 0;

  /**
   * \brief Get the ID of the entity, which is unique among all entities of the
   * same type in simulation. For real robots, it doesn't have to be unique, but
   * it probably still should be to assist with debugging.
   */
  virtual rtypes::type_uuid entity_id(void) const = 0;

  /* movement metrics */
  rtypes::spatial_dist distance(void) const override RCSW_PURE;
  rmath::vector2d velocity(void) const override;

  /* swarm spatial 2D metrics */
  const rmath::vector2d& position2D(void) const override final RCSW_PURE;
  const rmath::vector2u& discrete_position2D(void) const override final RCSW_PURE;
  rmath::vector2d heading2D(void) const override final RCSW_PURE;

  /**
   * \brief Set whether or not a robot is supposed to display it's ID above its
   * head during simulation.
   */
  void display_id(bool display_id) { m_display_id = display_id; }

  /**
   * \brief If \c TRUE, then the robot should display its ID above its head
   * during simulation.
   */
  bool display_id(void) const { return m_display_id; }

  /**
   * \brief Set the current clock tick.
   *
   * In a real world, each robot would maintain its own clock tick, and overall
   * there would no doubt be considerable skew; this is a simulation hack that
   * makes things much nicer/easier to deal with.
   */
  void tick(const rtypes::timestep& tick);

  /**
   * \brief Set the current location of the robot.
   *
   * This is a hack, as real world robot's would have to do their own
   * localization. This is far superior to that, in terms of ease of
   * programming. Plus it helps me focus in on my actual research. Ideally,
   * robots would calculate this from sensor values, rather than it being set by
   * the loop functions.
   */
  void position(const rmath::vector2d& loc);
  void discrete_position(const rmath::vector2u& loc);
  void heading(const rmath::radians& h);

#if (LIBRA_ER >= LIBRA_ER_ALL)
  /**
   * \brief Convenience function to add footbot ID to salient messages during
   * loop function execution (timestep is already there).
   */
  void ndc_push(void) const {
    ER_NDC_PUSH("[ent" + rcppsw::to_string(entity_id().v()) + "]");
  }
  void ndc_pop(void) const { ER_NDC_POP(); }
  /**
   * \brief Convenience function to add robot ID+timestep to messages during
   * the control step.
   */
  void ndc_pusht(void) const;
#else
  void ndc_pusht(void) const {}
  void ndc_pop(void) const {}
#endif

  /**
   * \brief Return a handle to the \ref rcppsw::rmath::rng used for random number
   * generation by this robot.
   */
  rmath::rng* rng(void) { return m_rng; }

 protected:
  /**
   * \brief Initialize controller output (i.e. where it will log events of
   * interest).
   *
   * \param output_root Absolute or relative path to the output root for the
   * robot.
   * \param output_dir Directory name within the output root that things should
   * be logged into. This is a separate argument than output_root, because there
   * are special values of it that have different behavior.
   *
   * Sets up the following log files in the output directory:
   *
   * - cosm.controller2D -> controller.log
   * - cosm.fsm -> fsm.log
   * - cosm.subsystem.saa -> saa.log
   *
   * \return Absolute path to the output directory.
   */
  std::string output_init(const std::string& output_root,
                          const std::string& output_dir);

  /**
   * \brief Initialize random number generation for the controller.
   *
   * \param seed The seed to use. -1 results in time seeded RNG, otherwise the
   *             seed value is used.
   * \param category The category of the RNG so that multiple robots can share
   *                 RNG (or not), depending on configuration.
   */
  void rng_init(int seed, const std::string& category);

  class subsystem::saa_subsystem2D* saa(void) {
    return m_saa.get();
  }
  const class subsystem::saa_subsystem2D* saa(void) const {
    return m_saa.get();
  }
  void saa(std::unique_ptr<subsystem::saa_subsystem2D> saa);

 private:
  /* clang-format off */
  bool                                        m_display_id{false};
  rmath::rng*                                 m_rng{nullptr};
  std::unique_ptr<subsystem::saa_subsystem2D> m_saa;
  /* clang-format on */
};

NS_END(controller, cosm);

#endif /* INCLUDE_COSM_CONTROLLER_BASE_CONTROLLER2D_HPP_ */
