/**
 * \file dist2D_pos_metrics_collector.cpp
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "cosm/spatial/metrics/dist2D_pos_metrics_collector.hpp"

#include "cosm/spatial/metrics/dist2D_metrics.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, spatial, metrics);

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void dist2D_pos_metrics_collector::collect(const rmetrics::base_metrics& metrics) {
  auto& m = dynamic_cast<const dist2D_metrics&>(metrics);
  inc_total_count();
  inc_cell_count(m.dpos2D());
} /* collect() */

NS_END(metrics, spatial, cosm);
