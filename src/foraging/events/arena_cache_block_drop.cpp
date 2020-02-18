/**
 * \file arena_cache_block_drop.cpp
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
#include "cosm/foraging/events/arena_cache_block_drop.hpp"

#include "cosm/ds/cell2D.hpp"
#include "cosm/foraging/ds/arena_map.hpp"
#include "cosm/foraging/events/arena_block_drop.hpp"
#include "cosm/foraging/repr/arena_cache.hpp"
#include "cosm/repr/base_block2D.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, foraging, events);
using cds::arena_grid;

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/

arena_cache_block_drop::arena_cache_block_drop(
    const std::shared_ptr<crepr::base_block2D>& arena_block,
    const std::shared_ptr<cfrepr::arena_cache>& cache,
    const rtypes::discretize_ratio& resolution)
    : ER_CLIENT_INIT("cosm.foraging.events.arena_cache_block_drop"),
      cell2D_op(cache->dloc()),
      mc_resolution(resolution),
      m_arena_block(arena_block),
      m_cache(cache) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void arena_cache_block_drop::visit(cds::cell2D& cell) {
  ER_ASSERT(0 != cell.loc().x() && 0 != cell.loc().y(),
            "Cell does not have coordinates");

  visit(cell.fsm());
  ER_ASSERT(m_cache->n_blocks() == cell.block_count(),
            "Cache/cell disagree on # of blocks: cache=%zu/cell=%zu",
            m_cache->n_blocks(),
            cell.block_count());
} /* visit() */

void arena_cache_block_drop::visit(fsm::cell2D_fsm& fsm) {
  ER_ASSERT(fsm.state_has_cache(), "Cell does not contain a cache");
  fsm.event_block_drop();
} /* visit() */

void arena_cache_block_drop::visit(cfds::arena_map& map) {
  map.block_mtx().lock();
  visit(*m_arena_block);
  map.block_mtx().unlock();

  /* Already holding cache mutex from calling context */
  visit(*m_cache);

  /*
   * Do not need to hold grid mutex because we know we are the only robot
   * picking up from the cache right now (though others can do it later) this
   * timestep, and caches by definition have a unique location, AND if another
   * robot has just caused a block re-distribution, that operation avoids
   * caches.
   */
  visit(map.access<arena_grid::kCell>(cell2D_op::x(), cell2D_op::y()));
  RCSW_UNUSED rtypes::type_uuid robot_id = m_arena_block->robot_id();

  ER_INFO("arena_map: fb%d dropped block%d in cache%d,total=[%s] (%zu)",
          robot_id.v(),
          m_arena_block->id().v(),
          m_cache->id().v(),
          rcppsw::to_string(m_cache->blocks()).c_str(),
          m_cache->n_blocks());
} /* visit() */

void arena_cache_block_drop::visit(crepr::base_block2D& block) {
  auto visitor = events::arena_block_drop_visitor::for_block(rmath::vector2u(cell2D_op::x(),
                                                                             cell2D_op::y()),
                                                             mc_resolution);
  visitor.visit(block);
} /* visit() */

void arena_cache_block_drop::visit(cfrepr::arena_cache& cache) {
  cache.block_add(m_arena_block);
  cache.has_block_drop();
} /* visit() */

NS_END(events, foraging, cosm);
