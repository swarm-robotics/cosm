/**
 * \file block_extent_set.hpp
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

#ifndef INCLUDE_COSM_ARENA_OPERATIONS_BLOCK_EXTENT_SET_HPP_
#define INCLUDE_COSM_ARENA_OPERATIONS_BLOCK_EXTENT_SET_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/er/client.hpp"
#include "rcppsw/math/vector2.hpp"
#include "rcppsw/mpl/typelist.hpp"
#include "rcppsw/patterns/visitor/visitor.hpp"

#include "cosm/cosm.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
namespace cosm::repr {
class base_block3D;
}

namespace cosm::ds {
class arena_grid;
} /* namespace cosm::ds */

NS_START(cosm, arena, operations, detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class block_extent_set
 * \ingroup arena operations
 *
 * \brief Set the cells that a block covers while in the arena that are in
 * an empty state to the BLOCK_EXTENT state.
 *
 * \note This operation requires holding the block and grid mutexes in
 *       multithreaded contexts.
 */
class block_extent_set : public rer::client<block_extent_set> {
 private:
  struct visit_typelist_impl {
    using value = rmpl::typelist<cds::arena_grid>;
  };

 public:
  using visit_typelist = visit_typelist_impl::value;

  explicit block_extent_set(crepr::base_block3D* block);
  block_extent_set& operator=(const block_extent_set&) = delete;
  block_extent_set(const block_extent_set&) = delete;

  void visit(cds::arena_grid& grid);

 private:
  /* clang-format off */
  crepr::base_block3D* m_block;
  /* clang-format on */
};

NS_END(detail);


/**
 * \brief We use the precise visitor in order to force compile errors if a call
 * to a visitor is made that involves a visitee that is not in our visit set
 * (i.e. remove the possibility of implicit upcasting performed by the
 * compiler).
 */
using block_extent_set_visitor = rpvisitor::filtered_visitor<detail::block_extent_set>;

NS_END(operations, arena, cosm);

#endif /* INCLUDE_COSM_ARENA_OPERATIONS_BLOCK_EXTENT_SET_HPP_ */
