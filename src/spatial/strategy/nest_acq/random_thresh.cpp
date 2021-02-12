/**
 * \file random_thresh.cpp
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
#include "cosm/spatial/strategy/nest_acq/random_thresh.hpp"

#include "cosm/subsystem/saa_subsystemQ3D.hpp"
#include "cosm/spatial/fsm/point_argument.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(cosm, spatial, strategy, nest_acq);

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void random_thresh::task_start(cta::taskable_argument* arg) {
  m_nest_loc = static_cast<fsm::point_argument*>(arg)->point();

  auto dist_to_light = (saa()->sensing()->rpos2D() - m_nest_loc).length();
  m_thresh = rtypes::spatial_dist(rng()->uniform(0.01, dist_to_light));
  m_task_running = true;
} /* task_start() */

void random_thresh::task_execute(void) {
  auto* ground =
      saa()->sensing()->template sensor<hal::sensors::ground_sensor>();
  /*
   * We might get pushed out of the nest by collision avoidance after initially
   * entering it.
   */
  if (ground->detect(hal::sensors::ground_sensor::kNestTarget)) {
    auto dist_to_light = (saa()->sensing()->rpos2D() - m_nest_loc).length();
    if (dist_to_light <= m_thresh.v()) {
      m_task_running = false;
    }
  }

  phototaxis();
  handle_ca();
} /* task_execute() */

NS_END(nest_acq, spatial, strategy, cosm);