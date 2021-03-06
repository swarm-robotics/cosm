/**
 * \file diff_drive_actuator.hpp
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

#ifndef INCLUDE_COSM_HAL_ACTUATORS_DIFF_DRIVE_ACTUATOR_HPP_
#define INCLUDE_COSM_HAL_ACTUATORS_DIFF_DRIVE_ACTUATOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/rcppsw.hpp"
#include "cosm/hal/hal.hpp"
#include "rcsw/common/fpc.h"

#if COSM_HAL_TARGET == HAL_TARGET_ARGOS_FOOTBOT
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#else
#error "Selected hardware has no differential drive actuator!"
#endif /* HAL_TARGET */

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(cosm, hal, actuators, detail);

/*******************************************************************************
 * Templates
 ******************************************************************************/
template<typename TSensor>
using is_argos_ds_actuator = std::is_same<TSensor,
                                          argos::CCI_DifferentialSteeringActuator>;

NS_END(detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class diff_drive_actuator_impl
 * \ingroup hal actuators
 *
 * \brief Differential drive actuator wrapper.
 *
 * Supports the following robots:
 *
 * - ARGoS footbot
 *
* \tparam TActuator The underlying actuator handle type abstracted away by the
 * HAL. If nullptr, then that effectively disables the actuator at compile time,
 * and SFINAE ensures no member functions can be called.
 */
template <typename TActuator>
class diff_drive_actuator_impl {
 public:
  /**
   * \brief Construct the wrapper actuator.
   *
   * \param wheels The underlying actuator. If NULL, then that effectively
   *               disables the actuator, and therefore subsequently calling any
   *               member function will have no effect.
   */
  explicit diff_drive_actuator_impl(TActuator* const wheels) : m_wheels(wheels) {}

  /**
   * \brief Set the wheel speeds for the current timestep for a footbot
   * robot. Bounds checking is not performed.
   */
  template <typename U = TActuator,
            RCPPSW_SFINAE_FUNC(detail::is_argos_ds_actuator<U>::value)>
  void set_wheel_speeds(double left, double right) {
    RCPPSW_FPC_RET_V(nullptr != m_wheels);
    m_wheels->SetLinearVelocity(left, right);
  }

  /**
   * \brief Stop the wheels of a footbot robot. As far as I know, this is an
   * immediate stop (i.e. no rampdown).
   */
  template <typename U = TActuator,
            RCPPSW_SFINAE_FUNC(detail::is_argos_ds_actuator<U>::value)>
  void reset(void) { set_wheel_speeds(0.0, 0.0); }

 private:
  /* clang-format off */
  TActuator* const m_wheels;
  /* clang-format on */
};

#if COSM_HAL_TARGET == HAL_TARGET_ARGOS_FOOTBOT
using diff_drive_actuator = diff_drive_actuator_impl<argos::CCI_DifferentialSteeringActuator>;
#endif /* HAL_TARGET */

NS_END(actuators, hal, cosm);

#endif /* INCLUDE_COSM_HAL_ACTUATORS_DIFF_DRIVE_ACTUATOR_IMPL_HPP_ */
