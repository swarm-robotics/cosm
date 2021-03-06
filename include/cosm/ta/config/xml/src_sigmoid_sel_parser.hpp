/**
 * \file src_sigmoid_sel_parser.hpp
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

#ifndef INCLUDE_COSM_TA_CONFIG_XML_SRC_SIGMOID_SEL_PARSER_HPP_
#define INCLUDE_COSM_TA_CONFIG_XML_SRC_SIGMOID_SEL_PARSER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <memory>

#include "rcppsw/common/common.hpp"
#include "rcppsw/config/xml/xml_config_parser.hpp"
#include "cosm/ta/config/xml/sigmoid_sel_parser.hpp"
#include "cosm/ta/config/src_sigmoid_sel_config.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(cosm, ta, config, xml);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class src_sigmoid_sel_parser
 * \ingroup ta config xml
 *
 * \brief Parses XML configuration relating to sourced sigmoid selection of
 * stuff into \ref src_sigmoid_sel_config.
 */
class src_sigmoid_sel_parser : public rcppsw::config::xml::xml_config_parser {
 public:
  using config_type = src_sigmoid_sel_config;

  /**
   * \brief The root tag that all XML configuration should lie under in the XML
   * tree.
   */
  static constexpr const char kXMLRoot[] = "src_sigmoid_sel";

  void parse(const ticpp::Element& node) override RCPPSW_COLD;
  bool validate(void) const override RCPPSW_ATTR(pure, cold);
  RCPPSW_COLD std::string xml_root(void) const override { return kXMLRoot; }

 private:
  RCPPSW_COLD rcppsw::config::base_config* config_get_impl(void) const override {
    return m_config.get();
  }

  /* clang-format off */
  std::unique_ptr<config_type> m_config{nullptr};
  sigmoid_sel_parser           m_sigmoid{};
  /* clang-format on */
};

NS_END(xml, config, ta, cosm);

#endif /* INCLUDE_COSM_TA_CONFIG_XML_SRC_SIGMOID_SEL_PARSER_HPP_ */
