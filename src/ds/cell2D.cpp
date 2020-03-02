/**
 * \file cell2D.cpp
 *
 * \copyright 2017 John Harwell, All rights reserved.
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
#include "cosm/ds/cell2D.hpp"

#include "cosm/foraging/repr/base_cache.hpp"
#include "cosm/repr/base_block2D.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, ds);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
cell2D::cell2D(void) { decoratee().init(); }

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
std::shared_ptr<crepr::base_block2D> cell2D::block(void) const {
  return std::dynamic_pointer_cast<crepr::base_block2D>(m_entity);
} /* block() */

std::shared_ptr<crepr::base_block2D> cell2D::block(void) {
  return std::dynamic_pointer_cast<crepr::base_block2D>(m_entity);
} /* block() */

std::shared_ptr<cfrepr::base_cache> cell2D::cache(void) {
  return std::dynamic_pointer_cast<cfrepr::base_cache>(m_entity);
} /* cache() */

std::shared_ptr<cfrepr::base_cache> cell2D::cache(void) const {
  return std::dynamic_pointer_cast<cfrepr::base_cache>(m_entity);
} /* cache() */

NS_END(ds, cosm);