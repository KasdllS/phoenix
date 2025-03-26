#pragma once

#include "parser/parser_impl.h"
#include "utils/meta.hpp"

namespace parser {

class Yaml {
 public:
  Yaml() = default;

  constexpr static char key[] = "yaml";

  static std::string serialize(const utils::Node&);
  static utils::Node deserialize(const std::string&);
};

}  // namespace parser