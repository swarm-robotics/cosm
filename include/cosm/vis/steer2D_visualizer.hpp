/**
 * \file steer2D_visualizer.hpp
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
#ifndef INCLUDE_COSM_VIS_STEER2D_VISUALIZER_HPP_
#define INCLUDE_COSM_VIS_STEER2D_VISUALIZER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>

#include <argos3/core/utility/math/quaternion.h>

#include "rcppsw/er/client.hpp"
#include "rcppsw/math/vector3.hpp"

#include "cosm/cosm.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace argos {
class CQTOpenGLUserFunctions;
}

namespace cosm::steer2D {
class tracker;
} /* namespace cosm::steer2D */

NS_START(cosm, vis);

/*******************************************************************************
 * Classes
 ******************************************************************************/
/**
 * \class steer2D_visualizer
 * \ingroup cosm vis
 *
 * \brief Renders one or more of the following:
 *
 * - The vectors representing individual 2D steering forces active that timestep
 *   for the robot, and their cumulative sum.
 *
 * - The path the robot is currently following, if applicable.
 */
class steer2D_visualizer : public rer::client<steer2D_visualizer> {
 public:
  steer2D_visualizer(argos::CQTOpenGLUserFunctions* qt, double text_vis_offset)
      : ER_CLIENT_INIT("cosm.vis.steer2D_visualizer"),
        m_text_vis_offset(text_vis_offset),
        m_qt(qt) {}

  steer2D_visualizer(const steer2D_visualizer&) = delete;
  steer2D_visualizer& operator=(const steer2D_visualizer&) = delete;

  void operator()(const rmath::vector3d& pos,
                  const argos::CQuaternion& orientation,
                  const steer2D::tracker* tracker);

 private:
  /* draw a little off the ground so it renders better */
  static constexpr const double kDRAW_OFFSET = 0.05;

  /**
   * \brief Draw 2D steering force visualizations
   *
   * \param tracker The 2D steering force tracker.
   */
  void forces_draw(const steer2D::tracker* tracker);

  /**
   * \brief Draw path visualizations
   *
   * \param pos The robot's current position.
   * \param azimuth The robot's current azimuth.
   * \param tracker The 2D steering force tracker.
   */
  void path_draw(const rmath::vector3d& pos,
                 const argos::CQuaternion& orientation,
                 const steer2D::tracker* tracker);

  /* clang-format off */
  double                               m_text_vis_offset{0.0};
  argos::CQTOpenGLUserFunctions* const m_qt{nullptr};
  /* clang-format on */
};

NS_END(cosm, vis);

#endif /* INCLUDE_COSM_VIS_STEER2D_VISUALIZER_HPP_ */
