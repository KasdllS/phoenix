#include <gtest/gtest.h>

#include "core/object.h"

#include <iostream>

struct MainObject : public core::Object {};

TEST(Object, Type) {
  std::string type = std::make_shared<core::Object>()->type();
  EXPECT_EQ(type, "core::Object");
  EXPECT_EQ(std::make_shared<MainObject>()->type(), "MainObject");
}