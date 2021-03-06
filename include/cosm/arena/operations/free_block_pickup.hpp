/**
 * \file free_block_pickup.hpp
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

#ifndef INCLUDE_COSM_ARENA_OPERATIONS_FREE_BLOCK_PICKUP_HPP_
#define INCLUDE_COSM_ARENA_OPERATIONS_FREE_BLOCK_PICKUP_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/er/client.hpp"
#include "rcppsw/types/type_uuid.hpp"
#include "rcppsw/types/timestep.hpp"

#include "cosm/ds/operations/cell2D_op.hpp"
#include "cosm/arena/arena_map_locking.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace cosm::arena {
class base_arena_map;
} /* namespace cosm::arena */

namespace cosm::repr {
class base_block3D;
} /* namespace cosm::repr */
namespace cosm::ds {
class arena_grid;
} /* namespace cosm::ds */

NS_START(cosm, arena, operations, detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class free_block_pickup
 * \ingroup arena operations detail
 *
 * \brief Fired whenever a robot picks up a free block in the arena (i.e. one
 * that is not part of a cache), OR when a block moves within the arena.
 */
class free_block_pickup : public rer::client<free_block_pickup>,
                          public cdops::cell2D_op {
 private:
  struct visit_typelist_impl {
    using value = rmpl::typelist<carena::base_arena_map>;
  };

 public:
  using visit_typelist = visit_typelist_impl::value;

  static free_block_pickup by_robot(crepr::base_block3D* block,
                                    const rtypes::type_uuid& robot_id,
                                    const rtypes::timestep& t,
                                    const arena_map_locking& locking);

  static free_block_pickup by_arena(crepr::base_block3D* block);

  ~free_block_pickup(void) override = default;

  free_block_pickup(const free_block_pickup&) = delete;
  free_block_pickup& operator=(const free_block_pickup&) = delete;

  /**
   * \brief Perform actual block pickup in the arena.
   *
   * Takes arena map grid mutex to protect grid updates. arena map block mutex
   * assumed to be held when calling this function.
   */
  void visit(base_arena_map& map);
  void visit(cds::arena_grid& grid);

 private:
  free_block_pickup(crepr::base_block3D* block,
                    const rtypes::type_uuid& robot_id,
                    const rtypes::timestep& t,
                    const arena_map_locking& locking);

  /* clang-format off */
  const rtypes::type_uuid mc_robot_id;
  const rtypes::timestep  mc_timestep;
  const arena_map_locking mc_locking;

  crepr::base_block3D*    m_block;
  /* clang-format on */
};

/**
 * \brief We use the precise visitor in order to force compile errors if a call
 * to a visitor is made that involves a visitee that is not in our visit set
 * (i.e. remove the possibility of implicit upcasting performed by the
 * compiler).
 */
using free_block_pickup_visitor_impl =
    rpvisitor::precise_visitor<free_block_pickup,
                               free_block_pickup::visit_typelist>;

NS_END(detail);

class free_block_pickup_visitor : public detail::free_block_pickup_visitor_impl {
 public:
  using detail::free_block_pickup_visitor_impl::free_block_pickup_visitor_impl;
};


NS_END(operations, arena, cosm);

#endif /* INCLUDE_COSM_ARENA_OPERATIONS_FREE_BLOCK_PICKUP_HPP_ */
