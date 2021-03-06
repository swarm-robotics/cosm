/**
 * \file nest_vector.cpp
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
#include "cosm/arena/ds/nest_vector.hpp"

#include <numeric>

#include "cosm/repr/nest.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, arena, ds);

/*******************************************************************************
 * Non-Member Functions
 ******************************************************************************/
template <typename TVector>
std::string do_to_str(const TVector& vec, bool full) {
  return std::accumulate(vec.begin(),
                         vec.end(),
                         std::string(),
                         [&](const std::string& a, const auto* n) {
                           return a + n->to_str(full);
                         });
} /* do_to_str() */

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
std::string nest_vectorno::to_str(bool full) const {
  return do_to_str(*this, full);
} /* to_str() */

std::string nest_vectorro::to_str(bool full) const {
  return do_to_str(*this, full);
} /* to_str() */

NS_END(ds, arena, cosm);
