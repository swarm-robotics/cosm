/**
 * \file block_list.hpp
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

#ifndef INCLUDE_COSM_FORAGING_DS_BLOCK_LIST_HPP_
#define INCLUDE_COSM_FORAGING_DS_BLOCK_LIST_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <list>
#include <string>
#include <memory>

#include "cosm/cosm.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace cosm::repr {
class base_block2D;
} /* namespace repr */

NS_START(cosm, foraging, ds);

using block_list_type = std::shared_ptr<crepr::base_block2D>;
using const_block_list_type = std::shared_ptr<const crepr::base_block2D>;

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class block_list
 * \ingroup foraging ds
 *
 * \brief Specialization of \ref std::list with an additional \ref to_str()
 * method.
 */
class block_list : public std::list<block_list_type> {
 public:
  using std::list<block_list_type>::list;
  using value_type = std::list<block_list_type>::value_type;

  /**
   * \brief Get a string representation of the list contents.
   */
  std::string to_str(void) const;
};

/**
 * \class const_block_list
 * \ingroup foraging ds
 *
 * \brief Specialization of \ref std::list with an additional \ref to_str()
 * method.
 */
class const_block_list : public std::list<const_block_list_type> {
 public:
  using std::list<const_block_list_type>::list;
  using value_type = std::list<const_block_list_type>::value_type;

  /**
   * \brief Get a string representation of the list contents.
   */
  std::string to_str(void) const;
};

NS_END(ds, foraging, cosm);

#endif /* INCLUDE_COSM_FORAGING_DS_BLOCK_LIST_HPP_ */