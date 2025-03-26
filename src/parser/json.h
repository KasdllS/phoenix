#pragma once

#include "parser/parser_impl.h"
#include "utils/meta.hpp"

namespace parser {

class Json {
 public:
  Json() = default;

  constexpr static char key[] = "json";
  static utils::Node deserialize(const std::string&);
  static std::string serialize(const utils::Node&);
};

}  // namespace parser