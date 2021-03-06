/**
 * \file block_cluster.cpp
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
#include "cosm/foraging/repr/block_cluster.hpp"

#include "cosm/repr/base_block3D.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, foraging, repr);

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void block_cluster::blocks_recalc(void) {
  m_blocks.clear();
  for (size_t i = 0; i < xdsize(); ++i) {
    for (size_t j = 0; j < ydsize(); ++j) {
      auto& cell = block_cluster::cell(i, j);
      ER_ASSERT(!cell.state_has_cache(),
                "Cell@%s in HAS_CACHE state",
                rcppsw::to_string(cell.loc()).c_str());
      ER_ASSERT(!cell.state_in_cache_extent(),
                "Cell@%s in CACHE_EXTENT state",
                rcppsw::to_string(cell.loc()).c_str());
      if (cell.state_has_block()) {
        ER_ASSERT(nullptr != cell.block3D(),
                  "Cell@%s null block3D",
                  rcppsw::to_string(cell.loc()).c_str());
        m_blocks.push_back(cell.block3D());
      }
    } /* for(j..) */
  } /* for(i..) */
} /* blocks_recalc() */

void block_cluster::update_after_drop(const crepr::base_block3D* dropped) {
  ER_ASSERT(contains_cell2D(dropped->danchor2D()),
            "Block%s@%s not contained in cluster%s extent",
            rcppsw::to_string(dropped->id()).c_str(),
            rcppsw::to_string(dropped->danchor2D()).c_str(),
            rcppsw::to_string(this->id()).c_str());

  auto relative_to = dropped->danchor2D() - danchor2D();

  auto& cell = block_cluster::cell(relative_to);
  ER_ASSERT(cell.state_has_block(),
            "Cell@%s not in HAS_BLOCK state",
            rcppsw::to_string(dropped->danchor2D()).c_str());
  ER_ASSERT(dropped->id() == cell.block3D()->id(),
            "Cell@%s block%s != dropped block%s",
            rcppsw::to_string(cell.loc()).c_str(),
            rcppsw::to_string(cell.block3D()->id()).c_str(),
            rcppsw::to_string(dropped->id()).c_str());
  m_blocks.push_back(dropped);
} /* update_after_drop() */

void block_cluster::update_after_pickup(const rtypes::type_uuid& pickup_id) {
  auto it = std::find_if(m_blocks.begin(),
                         m_blocks.end(),
                         [&](const auto* b) { return b->id() == pickup_id;});
  ER_ASSERT(it != m_blocks.end(),
            "Block%s not in cluster%s",
            rcppsw::to_string(pickup_id).c_str(),
            rcppsw::to_string(this->id()).c_str());
  m_blocks.erase(std::remove(m_blocks.begin(), m_blocks.end(), *it));
} /* update_after_pickup() */

NS_END(repr, foraging, cosm);
