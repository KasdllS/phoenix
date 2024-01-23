#include <gtest/gtest.h>

#include "utils/enum/enum_str.hpp"
#include "utils/meta/define.h"

using namespace phoenix;

Meta* meta = nullptr;

TEST(Meta, Bool) {
  using TestType = MetaImpl<bool>;
  meta = new TestType(false, false);
  EXPECT_EQ(((TestType*)(meta))->val_, false);
}

TEST(Meta, Enum) {
  enum class Type {
    Test1,
    Test2,
  };
  using TestType = MetaImpl<Type>;

  std::map<int64_t, std::string> combos;
  meta = new TestType(0, combos, false);
  EXPECT_EQ(((TestType*)(meta))->val_, 0);
}

TEST(Meta, Int) {
  using TestType = MetaImpl<int>;
  meta = new TestType(101, 100, 200, 100, false);
  EXPECT_EQ(((TestType*)(meta))->val_, 101);
}

TEST(Meta, Float) {
  using TestType = MetaImpl<double>;
  meta = new TestType(1.123, -0.1, 2., 0.5, false);
  EXPECT_EQ(((TestType*)(meta))->val_, 1.123);
}

TEST(Meta, String) {
  using TestType = MetaImpl<std::string>;
  meta = new TestType("hello", false);
  EXPECT_EQ(((TestType*)(meta))->val_, "hello");
}