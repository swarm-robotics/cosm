/**
 * \file cell2D_state.hpp
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

#ifndef INCLUDE_COSM_FSM_CELL2D_STATE_HPP_
#define INCLUDE_COSM_FSM_CELL2D_STATE_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/common/common.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(cosm, fsm);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \brief The state that a \ref cell2D_fsm can be in.
 */
class cell2D_state {
 public:
  enum {
    /**
     * \brief The cell's contents is unknown.
     */
    ekST_UNKNOWN,

    /**
     * \brief The cell is empty (does not hold a cache or block).
     */
    ekST_EMPTY,

    /**
     * \brief The cell contains a block.
     */
    ekST_HAS_BLOCK,

    /**
     * \brief The cell contains a cache.
     */
    ekST_HAS_CACHE,

    /**
     * \brief The cell does not contain a block, but is part of the 2D extent
     * occupied by a block, in which case it contains a reference to the block
     * it is part of.
     */
    ekST_BLOCK_EXTENT,

    /**
     * \brief The cell does not contain a cache, but is part of the 2D space
     * occupied by a cache, in which case it contains a reference to the
     * cache it is a part of.
     */
    ekST_CACHE_EXTENT,

    /**
     * \brief The cell does not contain a nest, but is part of the 2D extent
     * occupied by a nest, in which case it contains a reference to the nest it
     * is a part of.
     */
    ekST_NEST_EXTENT,

    ekST_MAX_STATES
  };
};

NS_END(cosm, fsm);

#endif /* INCLUDE_COSM_FSM_CELL2D_STATE_HPP_ */
