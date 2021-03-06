/**
 * \file movement_category.hpp
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

#ifndef INCLUDE_COSM_SPATIAL_METRICS_MOVEMENT_CATEGORY_HPP_
#define INCLUDE_COSM_SPATIAL_METRICS_MOVEMENT_CATEGORY_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "cosm/cosm.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(cosm, spatial, metrics);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \brief For use in attaching semantic meaning to different categorys of robot
 * motion as part of model validation.
 */
enum movement_category {
  /**
   * \brief Robots which are returning to a nest via the homing action.
   */
  ekHOMING,

  /**
   * \brief Robots which are exploring.
   */
  ekEXPLORING,

  /**
   * \brief Catch-all category which is reported each timestep.
   */
  ekALL,
  ekMAX
};

NS_END(metrics, spatial, cosm);

#endif /* INCLUDE_COSM_SPATIAL_METRICS_MOVEMENT_CATEGORY_HPP_ */
