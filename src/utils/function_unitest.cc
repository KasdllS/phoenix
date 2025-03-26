#include <gtest/gtest.h>
#include <iostream>
#include "utils/function.hpp"
#include "utils/time.hpp"

TEST(Function, Construct) {
  auto func = utils::Function::makeFunction(
      std::function<int(int)>([](int a) { return a; }));
  auto res = func.getResult<int>();
  func.invoke(1);
  EXPECT_EQ(res.get(), 1);
}

TEST(Function, Aysnc) {
  auto func = utils::Function::makeFunction(
      std::function<int(int, int)>([](int a, int b) { return a + b; }));
  auto res = func.getResult<int>();
  utils::time::tic();
  std::thread thd([&func]() { func.invoke(1, 2); });
  res.wait();
  EXPECT_EQ(3, res.get());
  if (thd.joinable()) {
    thd.join();
  }
}