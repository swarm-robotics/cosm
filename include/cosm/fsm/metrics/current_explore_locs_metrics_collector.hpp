/**
 * \file current_explore_locs_metrics_collector.hpp
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

#ifndef INCLUDE_COSM_FSM_METRICS_CURRENT_EXPLORE_LOCS_METRICS_COLLECTOR_HPP_
#define INCLUDE_COSM_FSM_METRICS_CURRENT_EXPLORE_LOCS_METRICS_COLLECTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <list>

#include "rcppsw/metrics/spatial/grid2D_avg_metrics_collector.hpp"
#include "cosm/cosm.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, fsm, metrics);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class current_explore_locs_metrics_collector
 * \ingroup fsm metrics
 *
 * \brief Collector for robot exploration trajectories, which is collected as a
 * 2D array, and needs its own collector separate from the \ref
 * goal_acq_metrics_collector (1 .csv per collector).
 */
class current_explore_locs_metrics_collector final : public rmetrics::spatial::grid2D_avg_metrics_collector {
 public:
  /**
   * \param ofname The output file name.
   * \param interval Collection interval.
   * \param dims Dimensions of the arena.
   */
  current_explore_locs_metrics_collector(const std::string& ofname,
                                    uint interval,
                                    const rmath::vector2u& dims) :
      grid2D_avg_metrics_collector(ofname, interval, dims) {}

  void collect(const rmetrics::base_metrics& metrics) override;
};

NS_END(metrics, fsm, cosm);

#endif /* INCLUDE_COSM_FSM_METRICS_CURRENT_EXPLORE_LOCS_METRICS_COLLECTOR_HPP_ */
