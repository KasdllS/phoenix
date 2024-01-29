#include <gtest/gtest.h>

#include "utils/meta/meta_define.hpp"

using namespace phoenix;

Meta* meta = nullptr;

TEST(Meta, Bool) {
  using TestType = MetaImpl<bool>;
  meta = new TestType(false, false);
  EXPECT_EQ(((TestType*)(meta))->val_, false);
}

TEST(Meta, BoolRef) {
  bool ref = false;
  using TestType = MetaImpl<bool&>;
  meta = new TestType(ref, false);
  ((TestType*)(meta))->val_ = true;
  EXPECT_EQ(ref, true);
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

TEST(Meta, EnumRef) {
  enum class Type {
    Test1,
    Test2,
  };
  using TestType = MetaImpl<Type&>;

  int64_t val = 0;

  std::map<int64_t, std::string> combos;
  meta = new TestType(val, combos, false);
  ((TestType*)(meta))->val_ = 1;
  EXPECT_EQ(val, 1);
}

TEST(Meta, Int) {
  using TestType = MetaImpl<int>;
  meta = new TestType(101, 100, 200, 100, false);
  EXPECT_EQ(((TestType*)(meta))->val_, 101);
}

TEST(Meta, IntRef) {
  int val = 100;
  using TestType = MetaImpl<int&>;
  meta = new TestType(val, 100, 200, 100, false);
  ((TestType*)(meta))->val_ = 101;
  EXPECT_EQ(val, 101);
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