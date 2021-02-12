/**
 * \file base_strategy.cpp
 *
 * \copyright 2020 John Harwell, All rights reserved.
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
#include "cosm/spatial/strategy/base_strategy.hpp"

#include "cosm/subsystem/saa_subsystemQ3D.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(cosm, spatial, strategy);

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
base_strategy::base_strategy(params* const p) : base_strategy{ p->saa, p->rng } {}

base_strategy::base_strategy(subsystem::saa_subsystemQ3D* const saa,
                             rmath::rng* rng)
    : m_saa(saa), m_rng(rng), m_inta_tracker(m_saa->sensing()) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void base_strategy::phototaxis(void) {
  auto* light = saa()->sensing()->template sensor<hal::sensors::light_sensor>();
  saa()->steer_force2D().accum(
      saa()->steer_force2D().phototaxis(light->readings()));

} /* phototaxis() */

void base_strategy::wander(void) {
  saa()->steer_force2D().accum(saa()->steer_force2D().wander(rng()));
} /* wander() */

void base_strategy::handle_ca(void) {
  auto* prox =
      saa()->sensing()->template sensor<hal::sensors::proximity_sensor>();
  if (auto obs = prox->avg_prox_obj()) {
    inta_tracker()->inta_enter();
    saa()->steer_force2D().accum(saa()->steer_force2D().avoidance(*obs));
  } else {
    inta_tracker()->inta_exit();
  }
} /* handle_ca() */


NS_END(strategy, spatial, cosm);