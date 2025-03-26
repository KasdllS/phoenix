#pragma once

#include "utils/meta.hpp"

namespace parser {

class Parser {
 public:
  Parser() = default;

  static utils::Node deserialize(const std::string& format,
                                 const std::string& bytes);
  static std::string serialize(const std::string& format,
                               const utils::Node& node);

  static utils::Node read(const std::string& file);
  static bool write(const std::string& file, utils::Node& node);
};

}  // namespace parser