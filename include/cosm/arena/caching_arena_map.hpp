/**
 * \file caching_arena_map.hpp
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

#ifndef INCLUDE_COSM_ARENA_CACHING_ARENA_MAP_HPP_
#define INCLUDE_COSM_ARENA_CACHING_ARENA_MAP_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <memory>
#include <string>
#include <vector>

#include "cosm/arena/base_arena_map.hpp"
#include "cosm/arena/ds/cache_vector.hpp"
#include "cosm/arena/repr/arena_cache.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(cosm, arena);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class caching_arena_map
 * \ingroup ds
 *
 * \brief Decorates \ref base_arena_map to add the ability to manage caches.
 */
class caching_arena_map final : public rer::client<caching_arena_map>,
                                public base_arena_map {
 public:
  caching_arena_map(const caconfig::arena_map_config* config, rmath::rng* rng);
  ~caching_arena_map(void) override;

  /**
   * \brief Get the list of all the caches currently present in the arena and
   * active.
   */
  cads::acache_vectorno& caches(void) { return m_cachesno; }
  const cads::acache_vectorno& caches(void) const { return m_cachesno; }

  /**
   * \brief Get the # of caches currently in the arena.
   */
  size_t n_caches(void) const { return m_cacheso.size(); }

  /**
   * \brief Add caches that have been created (by a cache manager or by robots)
   * in the arena to the current set of active caches.
   *
   * \param caches The caches to add.
   * \param sm The \ref argos_sm_adaptor.
   *
   * \note Cache mutex assumed to be held by the caller for writing.
   */
  void caches_add(const cads::acache_vectoro& caches, pal::argos_sm_adaptor* sm);

  /**
   * \brief Remove a cache from the list of caches.
   *
   * \param victim The cache to remove.
   * \param sm The swarm manager (to remove light for cache).
   *
   * \note Cache mutex assumed to be held by the caller for writing.
   */
  void cache_remove(repr::arena_cache* victim, pal::argos_sm_adaptor* sm);

  const cads::loctree* cloctree(void) const { return m_cloctree.get(); }

  /**
   * \brief Determine if a robot is currently on top of a cache (i.e. if the
   * center of the robot has crossed over into the space occupied by the block
   * extent).
   *
   * While the robots also have their own means of checking if they are on a
   * cache or not, there are some false positives, so this function is used as
   * the final arbiter when deciding whether or not to trigger a cache related
   * event for a particular robot.
   *
   * \param pos The position of a robot.
   *
   * \return The ID of the cache that the robot is on, or -1 if the robot is not
   * actually on a cache.
   */
  rtypes::type_uuid robot_on_cache(const rmath::vector2d& pos) const RCPPSW_PURE;

  rtypes::type_uuid
  robot_on_block(const rmath::vector2d& pos,
                 const rtypes::type_uuid& ent_id) const override RCPPSW_PURE;

  /**
   * \brief Get the free blocks in the arena. Does no locking, so this is only
   * safe to call in non-concurrent contexts.
   */
  cds::block3D_vectorno free_blocks(void) const override;

  bool placement_conflict(const crepr::base_block3D* const block,
                          const rmath::vector2d& loc) const override;

  void bloctree_update(const crepr::base_block3D* block,
                       const arena_map_locking& locking,
                       const ds::acache_vectoro& created = {});
  void cloctree_update(const carepr::arena_cache* cache);
  /**
   * \brief Protects simultaneous updates to the caches vector.
   */
  std::shared_mutex* cache_mtx(void) { return &m_cache_mtx; }
  std::shared_mutex* cache_mtx(void) const { return &m_cache_mtx; }

  /**
   * \brief Clear the list of caches that have been removed this timestep.
   *
   * Having such a list is necessary in order to be able to correctly gather
   * metrics from caches that have been depleted THIS timestep about block
   * pickups. Normal cache metric collection does not encompass such zombie
   * caches.
   */
  void zombie_caches_clear(void) { m_zombie_caches.clear(); }
  const cads::acache_vectoro& zombie_caches(void) const {
    return m_zombie_caches;
  }

 private:
  void pre_block_dist_lock(const arena_map_locking& locking) override;
  void post_block_dist_unlock(const arena_map_locking& locking) override;
  block_dist_precalc_type
  block_dist_precalc(const crepr::base_block3D* block) override;
  bool bloctree_verify(void) const override;
  bool cloctree_verify(void) const;

  /* clang-format off */
  mutable std::shared_mutex              m_cache_mtx{};

  cads::acache_vectoro                   m_cacheso{};
  cads::acache_vectorno                  m_cachesno{};

  /**
   * Must be owning in order to correctly handle robot cached block pickup
   * events.
   */
  cads::acache_vectoro                   m_zombie_caches{};
  std::unique_ptr<cads::loctree>         m_cloctree;

  /* clang-format on */
};

NS_END(arena, cosm);

#endif /* INCLUDE_COSM_ARENA_CACHING_ARENA_MAP_HPP_ */
