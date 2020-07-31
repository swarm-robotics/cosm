/**
 * \file base_arena_block_pickup.hpp
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

#ifndef INCLUDE_COSM_INTERACTORS_BASE_ARENA_BLOCK_PICKUP_HPP_
#define INCLUDE_COSM_INTERACTORS_BASE_ARENA_BLOCK_PICKUP_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <boost/mpl/at.hpp>

#include <argos3/core/simulator/entity/floor_entity.h>

#include "cosm/arena/base_arena_map.hpp"
#include "cosm/arena/operations/free_block_pickup.hpp"
#include "cosm/tv/temporal_penalty.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, interactors);

/*******************************************************************************
 * Classes
 ******************************************************************************/
/**
 * \class base_arena_block_pickup
 * \ingroup interactors
 *
 * \brief Handle's (possible) free block pickup event on a given timestep,
 * updating the robot and the arena map state as needed if the conditions for
 * the pickup are met.
 *
 * \tparam TController The type of the robot controller

 * \tparam TControllerSpecMap Compiler time map mapping the type of the
 *         controller to a set of types specifying:
 *
 *         1. The type of the arena map to use.
 *         2. The type of the penalty handler for the free block pickup
 *            operation to use.
 *         3. The return type of the \p operator() function to use.
 *         4. The type of the block vanished visitor to use, for when the block
 *            a robot is trying to pickup vanishes as it is serving its penalty.
 *         5. The type of the visitor to visit the controller in order to
 *            process the block pickup on the robot side (arena map side is
 *            already generic).
 */
template <typename TController, typename TControllerSpecMap>
class base_arena_block_pickup
    : public rer::client<base_arena_block_pickup<TController, TControllerSpecMap>> {
 public:
  using controller_spec =
      typename boost::mpl::at<TControllerSpecMap, TController>::type;
  using arena_map_type = typename controller_spec::arena_map_type;
  using penalty_handler_type = typename controller_spec::penalty_handler_type;
  using interactor_status_type =
      typename controller_spec::interactor_status_type;
  using robot_block_vanished_visitor_type =
      typename controller_spec::robot_block_vanished_visitor_type;
  using robot_block_pickup_visitor_type =
      typename controller_spec::robot_block_pickup_visitor_type;

  base_arena_block_pickup(arena_map_type* const map,
                          argos::CFloorEntity* const floor,
                          penalty_handler_type* const penalty_handler)
      : ER_CLIENT_INIT("cosm.interactors.base_arena_block_pickup"),
        m_floor(floor),
        m_map(map),
        m_penalty_handler(penalty_handler) {}
  ~base_arena_block_pickup(void) override = default;

  base_arena_block_pickup(base_arena_block_pickup&&) = default;

  /* Not copy-constructible/assignable by default. */
  base_arena_block_pickup(const base_arena_block_pickup&) = delete;
  base_arena_block_pickup& operator=(const base_arena_block_pickup&) = delete;

  /**
   * \brief If the robot is not currently serving a penalty, then this callback
   * is used to attempt to initialized one. It is needed because the penalty
   * handler type is unknown, and therefore so are the arguments needed for
   * initializing the penalty, beyond the controller and the timestep.
   */
  virtual void robot_penalty_init(const TController& controller,
                                  const rtypes::timestep& t,
                                  penalty_handler_type* handler) = 0;

  /**
   * \brief Determine if the robot has acquired its goal (a block in this
   * case). This callback is needed because the exact parameters/method of
   * determining when this has occurred are project specific (e.g., checking if
   * the controller's acquisition_goal() function returns a certain integer).
   */
  virtual bool robot_goal_acquired(const TController& controller) const = 0;

  /**
   * \brief Called right before the robot is visited by the block pickup event,
   * in order to provide a hook for derived classes to update controller
   * bookkeeping before the pickup.
   */
  virtual void robot_previsit_hook(TController&,
                                   const ctv::temporal_penalty&) const {}

  /**
   * \brief Handle robot-arena interactions for the specified controller
   * instance on this timestep.
   *
   * \param controller The controller to handle interactions for.
   * \param t The current timestep.
   */
  interactor_status_type operator()(TController& controller,
                                    const rtypes::timestep& t) {
    if (m_penalty_handler->is_serving_penalty(controller)) {
      if (m_penalty_handler->is_penalty_satisfied(controller, t)) {
        process_pickup(controller, t);
        return interactor_status_type::ekARENA_FREE_BLOCK_PICKUP;
      }
    } else {
      robot_penalty_init(controller, t, m_penalty_handler);
    }
    return interactor_status_type::ekNO_EVENT;
  }

 private:
  /**
   * \brief Now that a robot has satisfied its penalty for block pickup,
   * determine if it is still on a free block, and if so execute the block
   * pickup.
   */
  void process_pickup(TController& controller, const rtypes::timestep& t) {
    ER_ASSERT(robot_goal_acquired(controller),
              "Controller not waiting for free block pickup");
    ER_ASSERT(m_penalty_handler->is_serving_penalty(controller),
              "Controller not serving pickup penalty");
    ER_ASSERT(!controller.is_carrying_block(),
              "Controller is already carrying block%d",
              controller.block()->id().v());
    /*
     * More than 1 robot can pick up a block in a timestep, so we have to
     * search for this robot's controller.
     */
    const auto& p = *m_penalty_handler->penalty_find(controller);

    /*
     * We cannot just lock around the critical arena map updates here in order
     * to make this section thread safe because if two threads updating two
     * robots both having finished serving their penalty this timestep manage to
     * pass the check to actually perform the block pickup before one of them
     * actually finishes picking up a block, then the second one will not get
     * the necessary \ref block_vanished event. See COSM#594.
     */
    m_map->block_mtx()->lock();

    /*
     * If two robots both are serving penalties on the same ramp block (possible
     * because a ramp block spans 2 squares), then whichever robot finishes
     * first will correctly take the block via \ref free_block_pickup, and the
     * second one will attempt to perform the pickup on a block that is already
     * out of sight, resulting in a boost index out of bounds assertion. See
     * COSM#410.
     *
     * Furthermore, it is ALSO possible that while the second robot is still
     * waiting to serve its penalty, and the first robot has already picked up
     * the ramp block, that the arena distributes a new block onto the square
     * that the robot is currently occupying. Thus, when it has served its
     * penalty, we need to check that the ID of the block the robot is on
     * matches the ID of the block we originally served the penalty for (not
     * just checking if it is not -1).
     */
    if (p.id() != m_map->robot_on_block(controller.rpos2D(),
                                        controller.entity_acquired_id())) {
      ER_WARN("%s cannot pickup block%d: No such block",
              controller.GetId().c_str(),
              m_penalty_handler->penalty_find(controller)->id().v());
      robot_block_vanished_visitor_type vanished(p.id());
      vanished.visit(controller);
    } else {
      execute_pickup(controller, p, t);
    }
    m_map->block_mtx()->unlock();

    m_penalty_handler->penalty_remove(p);
    ER_ASSERT(
        !m_penalty_handler->is_serving_penalty(controller),
        "Multiple instances of same controller serving block pickup penalty");
  }

  /**
   * \brief Perform the actual picking up of a free block once all
   * preconditions have been satisfied, updating both the arena map and the
   * robot, in that order.
   */
  void execute_pickup(TController& controller,
                      const ctv::temporal_penalty& penalty,
                      const rtypes::timestep& t) {
    /* Holding block mutex not necessary here, but does not hurt */
    auto it =
        std::find_if(m_map->blocks().begin(),
                     m_map->blocks().end(),
                     [&](const auto& b) { return b->id() == penalty.id(); });
    ER_ASSERT(it != m_map->blocks().end(),
              "Block%d from penalty does not exist?",
              penalty.id().v());
    ER_ASSERT(!(*it)->is_out_of_sight(),
              "Attempt to pick up out of sight block%d",
              (*it)->id().v());

    robot_block_pickup_visitor_type rpickup_op(*it, controller.entity_id(), t);
    auto apickup_op = caops::free_block_pickup_visitor::by_robot(*it,
                                                                 controller.entity_id(),
                                                                 t);

    /* update bookkeeping */
    robot_previsit_hook(controller, penalty);

    /*
     * Visitation order must be:
     *
     * 1. Arena map
     * 2. Controller
     *
     * In order for the pickup event to process properly.
     */
    apickup_op.visit(static_cast<carena::base_arena_map&>(*m_map));
    rpickup_op.visit(controller);

    /* The floor texture must be updated */
    m_floor->SetChanged();
  }

  /* clang-format off */
  argos::CFloorEntity*const   m_floor;
  arena_map_type* const       m_map;
  penalty_handler_type* const m_penalty_handler;
  /* clang-format on */
};

NS_END(interactors, cosm);

#endif /* INCLUDE_COSM_INTERACTORS_BASE_ARENA_BLOCK_PICKUP_HANDLER_HPP_ */
