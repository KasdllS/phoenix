#include "utils/meta.hpp"
#include "parser/parser.h"

#include <gtest/gtest.h>
#include <iostream>

const char yaml[] = "result:\n swing_a: 1.0\n swing_b: 0.0\nerror: [0.0, 0.1]";

TEST(YAML, Parser) {
  auto n = parser::Parser::deserialize("yaml", yaml);
  EXPECT_EQ(n.isValid(), true);
  EXPECT_EQ(n.isMap(), true);
  EXPECT_EQ(n["result"]["swing_a"].as<double>(), 1.0);
  EXPECT_EQ(n["error"][1].as<double>(), 0.1);
}