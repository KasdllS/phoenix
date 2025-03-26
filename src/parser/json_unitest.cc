#include "utils/meta.hpp"
#include "parser/parser.h"

#include <gtest/gtest.h>

const char json[] =
    "{\"result\":{\"swing_a\": 1.0, \"swing_b\": 0.0},\"error\": [0.0, 0.1]}";

TEST(Json, Parser) {
  auto n = parser::Parser::deserialize("json", json);
  EXPECT_EQ(n.isValid(), true);
  EXPECT_EQ(n.isMap(), true);
  EXPECT_EQ(n["result"]["swing_a"].as<double>(), 1.0);
  EXPECT_EQ(n["error"][1].as<double>(), 0.1);
}