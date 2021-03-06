/**
 * \file nest_config.hpp
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

#ifndef INCLUDE_COSM_REPR_CONFIG_NEST_CONFIG_HPP_
#define INCLUDE_COSM_REPR_CONFIG_NEST_CONFIG_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <vector>

#include "rcppsw/config/base_config.hpp"
#include "rcppsw/math/vector2.hpp"
#include "cosm/cosm.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, repr, config);

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * \struct nest_config
 * \ingroup repr config
 *
 * \brief Configuration for a single \ref nest within the arena.
 */
struct nest_config final : public rconfig::base_config {
  rmath::vector2d center{};
  rmath::vector2d dims{};
};

NS_END(config, repr, cosm);

#endif /* INCLUDE_COSM_REPR_CONFIG_NEST_CONFIG_HPP_ */
