/**
 * @file led_actuator.hpp
 *
 * @copyright 2018 John Harwell, All rights reserved.
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

#ifndef INCLUDE_COSM_HAL_ACTUATORS_LED_ACTUATOR_HPP_
#define INCLUDE_COSM_HAL_ACTUATORS_LED_ACTUATOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/rcppsw.hpp"
#include "rcppsw/utils/color.hpp"
#include "cosm/hal/hal.hpp"
#include "cosm/cosm.hpp"

#if COSM_HAL_TARGET == HAL_TARGET_ARGOS_FOOTBOT
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#else
#error "Selected component has no LEDs!"
#endif /* HAL_TARGET */

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(cosm, hal, actuators, detail);

/*******************************************************************************
 * Templates
 ******************************************************************************/
template<typename TActuator>
using is_argos_led_actuator = std::is_same<TActuator,
                                           argos::CCI_LEDsActuator>;

NS_END(detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class led_actuator_
 * @ingroup cosm hal
 *
 * @brief LED actuator wrapper for the following supported robots:
 *
 * - ARGoS footbot
 * - NULL robot (robot without LEDs to actuate). This is used to compile out
 *   the selected robot sensor, and as such does not have a preprocessor
 *   definition.
 */
template<typename T>
class _led_actuator {
 public:
  explicit _led_actuator(T* const leds) : m_leds(leds) {}

  /**
   * @brief Reset the LED actuator.
   */
  void reset(void) {}

  /**
   * @brief Set a single LED on the footbot robot to a specific color (or set
   * all LEDs to a specific color).
   *
   * @param id Which LED to change color. This is application defined. However,
   * the reserved value of -1 should be interpreted to mean set the color of
   * \c ALL LEDs on the robot.
   *
   * @param color The color to change the LED to. This is application defined.
   */
  template <typename U = T,
            RCPPSW_SFINAE_FUNC(detail::is_argos_led_actuator<U>::value)>
  void set_color(int id, const rutils::color& color) {
    if (nullptr == m_leds) {
      return;
    }
    if (-1 == id) {
      m_leds->SetAllColors(argos::CColor(color.red(),
                                         color.green(),
                                         color.blue(),
                                         color.alpha()));
    } else {
      m_leds->SetSingleColor(id, argos::CColor(color.red(),
                                               color.green(),
                                               color.blue(),
                                               color.alpha()));
    }
  }

  /**
   * @brief Set intensity for a single LED on the footbot robot (or set
   * intensity of all LEDs).
   *
   * @param id Which LED to set intensity for. This is application
   * defined. However, the reserved value of -1 should be interpreted to mean
   * set the intensity of \c ALL LEDs on the robot.
   *
   * @param intensity In the range [0,255]. Application defined meaning.
   */
  template <typename U = T,
            RCPPSW_SFINAE_FUNC(detail::is_argos_led_actuator<U>::value)>
  void set_intensity(int id, uint8_t intensity) {
    if (-1 == id) {
      m_leds->SetAllIntensities(intensity);
    } else {
      m_leds->SetSingleIntensity(id, intensity);
    }
  }

 private:
  /* clang-format off */
  T* m_leds;
  /* clang-format on */
};

#if COSM_HAL_TARGET == HAL_TARGET_ARGOS_FOOTBOT
using led_actuator = _led_actuator<argos::CCI_LEDsActuator>;
#endif /* HAL_TARGET */

NS_END(actuators, hal, cosm);

#endif /* INCLUDE_COSM_HAL_ACTUATORS_LED_ACTUATOR_HPP_ */
