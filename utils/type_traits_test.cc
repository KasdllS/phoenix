#include "utils/type_traits.hpp"

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

using namespace phoenix;

TEST(TypeTraits, IsVector) {
  EXPECT_EQ(is_vector_v<int>, false);
  EXPECT_EQ(is_vector_v<std::vector<int>>, true);
  EXPECT_EQ(is_vector_v<const std::vector<int>&>, false);
  EXPECT_EQ(is_vector_v<std::decay_t<const std::vector<int>&>>, true);
}

TEST(TypeTraits, IsMap) {
  EXPECT_EQ(is_map_v<int>, false);

  bool val = is_map_v<std::map<std::string, int>>;
  EXPECT_EQ(val, true);

  val = is_map_v<std::map<int, double>>;
  EXPECT_EQ(val, true);

  EXPECT_EQ(is_map_v<std::vector<int>>, false);
}