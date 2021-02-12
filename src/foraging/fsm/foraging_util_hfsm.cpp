/**
 * \file foraging_util_hfsm.cpp
 *
 * \copyright 2021 John Harwell, All rights reserved.
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "cosm/foraging/fsm/foraging_util_hfsm.hpp"

#include <numeric>

#include "cosm/spatial/fsm/util_signal.hpp"
#include "cosm/subsystem/actuation_subsystem2D.hpp"
#include "cosm/subsystem/saa_subsystemQ3D.hpp"
#include "cosm/subsystem/sensing_subsystemQ3D.hpp"
#include "cosm/spatial/fsm/point_argument.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, foraging, fsm);

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
foraging_util_hfsm::foraging_util_hfsm(csubsystem::saa_subsystemQ3D* const saa,
                                       std::unique_ptr<csstrategy::base_strategy> nest_acq,
                                       rmath::rng* rng,
                                       uint8_t max_states)
    : util_hfsm(saa, rng, max_states),
      ER_CLIENT_INIT("cosm.foraging.fsm.foraging_util_hfsm"),
      RCPPSW_HFSM_CONSTRUCT_STATE(transport_to_nest, hfsm::top_state()),
      RCPPSW_HFSM_CONSTRUCT_STATE(leaving_nest, hfsm::top_state()),
      m_nest_acq(std::move(nest_acq)) {}

/*******************************************************************************
 * States
 ******************************************************************************/

RCPPSW_HFSM_STATE_DEFINE(foraging_util_hfsm, leaving_nest, rpfsm::event_data* data) {
  ER_ASSERT(rpfsm::event_type::ekNORMAL == data->type(),
            "ekST_LEAVING_NEST cannot handle child events");

  if (current_state() != last_state()) {
    ER_DEBUG("Executing ekST_LEAVING_NEST");
  }
  /*
   * We don't want to just apply anti-phototaxis force, because that will make
   * the robot immediately turn around as soon as it has entered the nest and
   * dropped its block, leading to a lot of traffic jams by the edge of the
   * nest. Instead, wander about within the nest until you find the edge (either
   * on your own or being pushed out via collision avoidance).
   */
  auto* prox =
      saa()->sensing()->template sensor<hal::sensors::proximity_sensor>();
  if (auto obs = prox->avg_prox_obj()) {
    inta_tracker()->inta_enter();
    saa()->steer_force2D().accum(saa()->steer_force2D().avoidance(*obs));
  } else {
    inta_tracker()->inta_exit();
  }
  saa()->steer_force2D().accum(saa()->steer_force2D().wander(rng()));

  auto* ground = saa()->sensing()->template sensor<hal::sensors::ground_sensor>();
  if (!ground->detect(hal::sensors::ground_sensor::kNestTarget)) {
    return csfsm::util_signal::ekLEFT_NEST;
  }
  return rpfsm::event_signal::ekHANDLED;
} /* leaving_nest() */

RCPPSW_HFSM_STATE_DEFINE(foraging_util_hfsm,
                         transport_to_nest,
                         nest_transport_data* data) {
  ER_ASSERT(rpfsm::event_type::ekNORMAL == data->type(),
            "ekST_TRANSPORT_TO_NEST cannot handle child events");
  if (current_state() != last_state()) {
    ER_DEBUG("Executing ekST_TRANSPORT_TO_NEST");
    ER_ASSERT(nullptr != m_nest_acq, "NULL nest acquisition behavior");
  }
  event_data_hold(true);

  auto* ground = saa()->sensing()->template sensor<hal::sensors::ground_sensor>();

  if (!m_nest_acq->task_running()) {
    /* We have entered the nest, so perform our acquisition strategy */
    if (ground->detect(hal::sensors::ground_sensor::kNestTarget)) {
      csfsm::point_argument arg(-1, data->nest_loc); /* tolerance not used */
      m_nest_acq->task_reset();
      m_nest_acq->task_start(&arg);
    }
  }

  if (m_nest_acq->task_running()) {
    m_nest_acq->task_execute();
    if (m_nest_acq->task_finished()) {
      if (ground->detect(hal::sensors::ground_sensor::kNestTarget)) {
        /*
         * We have arrived at the nest so stop moving and signal.
         */
        saa()->actuation()->actuator<ckin2D::governed_diff_drive>()->reset();
        return csfsm::util_signal::ekENTERED_NEST;
      } else { /* we are somehow outside the nest--try again */
        m_nest_acq->task_reset();
      }
    }
  } else { /* still outside the nest, just phototaxis + collision avoidance */
    auto* prox =
        saa()->sensing()->template sensor<hal::sensors::proximity_sensor>();
    if (auto obs = prox->avg_prox_obj()) {
      inta_tracker()->inta_enter();
      saa()->steer_force2D().accum(saa()->steer_force2D().avoidance(*obs));
    } else {
      inta_tracker()->inta_exit();
    }
    auto* light = saa()->sensing()->template sensor<hal::sensors::light_sensor>();
    saa()->steer_force2D().accum(
        saa()->steer_force2D().phototaxis(light->readings()));
  }
  return rpfsm::event_signal::ekHANDLED;
} /* transport_to_nest() */

RCPPSW_HFSM_ENTRY_DEFINE_ND(foraging_util_hfsm, entry_leaving_nest) {
  actuation()->template actuator<hal::actuators::led_actuator>()->set_color(
      -1, rutils::color::kWHITE);
}

RCPPSW_HFSM_ENTRY_DEFINE_ND(foraging_util_hfsm, entry_transport_to_nest) {
  sensing()->template sensor<hal::sensors::light_sensor>()->enable();
  actuation()->template actuator<hal::actuators::led_actuator>()->set_color(
      -1, rutils::color::kGREEN);
}

RCPPSW_HFSM_EXIT_DEFINE(foraging_util_hfsm, exit_transport_to_nest) {
  sensing()->template sensor<hal::sensors::light_sensor>()->disable();
}

NS_END(fsm, foraging, cosm);