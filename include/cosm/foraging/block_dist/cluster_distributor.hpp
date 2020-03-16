/**
 * \file cluster_distributor.hpp
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

#ifndef INCLUDE_COSM_FORAGING_BLOCK_DIST_CLUSTER_DISTRIBUTOR_HPP_
#define INCLUDE_COSM_FORAGING_BLOCK_DIST_CLUSTER_DISTRIBUTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <memory>

#include "cosm/foraging/block_dist/random_distributor.hpp"
#include "cosm/foraging/block_dist/base_distributor.hpp"
#include "cosm/foraging/repr/block_cluster.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, foraging, block_dist);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class cluster_distributor
 * \ingroup foraging block_dist
 *
 * \brief Distributes a block or set of blocks within the specified cluster
 * bounds randomly, using \ref random_distributor.
 */
class cluster_distributor final : public rer::client<cluster_distributor>,
                                  public base_distributor {
 public:
  cluster_distributor(const cds::arena_grid::view& view,
                      const rtypes::discretize_ratio& resolution,
                      uint capacity,
                      rmath::rng* rng);
  ~cluster_distributor(void) override = default;

  cluster_distributor& operator=(const cluster_distributor& s) = delete;

  bool distribute_block(crepr::base_block2D* block,
                        cds::const_entity_list& entities) override;
  bool distribute_blocks(cds::block2D_vectorno& blocks,
                         cds::const_entity_list& entities) override;

  /**
   * \brief Return the \ref cfds::block_cluster associated with the cluster
   * distributer into which blocks will be distributed.
   */
  cfds::block_cluster_vector block_clusters(void) const override;

 private:
  /* clang-format off */
  cfrepr::block_cluster m_clust;
  random_distributor    m_impl;
  /* clang-format on */
};

NS_END(block_dist, foraging, cosm);

#endif /* INCLUDE_COSM_FORAGING_BLOCK_DIST_CLUSTER_DISTRIBUTOR_HPP_ */
