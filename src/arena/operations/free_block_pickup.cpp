/**
 * \file free_block_pickup.cpp
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
#include "cosm/arena/operations/free_block_pickup.hpp"

#include "cosm/arena/base_arena_map.hpp"
#include "cosm/arena/operations/block_extent_clear.hpp"
#include "cosm/ds/operations/cell2D_empty.hpp"
#include "cosm/repr/base_block3D.hpp"
#include "cosm/repr/operations/block_pickup.hpp"
#include "cosm/foraging/block_dist/base_distributor.hpp"
#include "cosm/foraging/repr/block_cluster.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, arena, operations, detail);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
free_block_pickup free_block_pickup::by_robot(crepr::base_block3D* block,
                                              const rtypes::type_uuid& robot_id,
                                              const rtypes::timestep& t,
                                              const arena_map_locking& locking) {
  return free_block_pickup(block, robot_id, t, locking);
} /* by_robot() */

free_block_pickup free_block_pickup::by_arena(crepr::base_block3D* block) {
  return free_block_pickup(block,
                           rtypes::constants::kNoUUID,
                           rtypes::timestep(-1),
                           arena_map_locking::ekALL_HELD);
} /* by_arena() */

free_block_pickup::free_block_pickup(crepr::base_block3D* block,
                                     const rtypes::type_uuid& robot_id,
                                     const rtypes::timestep& t,
                                     const arena_map_locking& locking)
    : ER_CLIENT_INIT("cosm.arena.operations.free_block_pickup"),
      cell2D_op(block->danchor2D()),
      mc_robot_id(robot_id),
      mc_timestep(t),
      mc_locking(locking),
      m_block(block) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void free_block_pickup::visit(cds::arena_grid& grid) {
  ER_ASSERT(!m_block->is_out_of_sight(),
            "Block%d out of sight on pickup",
            m_block->id().v());

  caops::block_extent_clear_visitor ec(m_block);
  cdops::cell2D_empty_visitor hc(coord());

  grid.mtx()->lock();

  /* mark host cell as empty (not done as part of clearing block extent) */
  hc.visit(grid);

  /* clear block extent */
  ec.visit(grid);

  grid.mtx()->unlock();

  if (rtypes::constants::kNoUUID != mc_robot_id) {
    /* Update block state--already holding block mutex if it is needed */
    crops::block_pickup block_op(mc_robot_id, mc_timestep);
    block_op.visit(*m_block, crops::block_pickup_owner::ekARENA_MAP);
  }
} /* visit() */

void free_block_pickup::visit(base_arena_map& map) {
    /* capture where the block used to be */
  rmath::vector2z old = m_block->danchor2D();

  /* update the arena grid */
  visit(map.decoratee());

  /* update block loctree */
  map.bloctree_update(m_block, mc_locking);

  /* update block clusters--the picked up block disappeared from one of them */
  auto clusters = map.block_distributor()->block_clustersno();
  for (auto *clust : clusters) {
    if (clust->contains_cell2D(old)) {
      clust->update_after_pickup(m_block->id());
      return;
    }
  } /* for(*clust..) */
  ER_FATAL_SENTINEL("Block%s not found in any block cluster?",
                    rcppsw::to_string(m_block->id()).c_str());
} /* visit() */

NS_END(detail, operations, arena, cosm);
