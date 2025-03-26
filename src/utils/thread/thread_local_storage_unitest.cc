#include <gtest/gtest.h>
#include <thread>
#include "utils/thread/thread_local_storage.hpp"

struct Data {
  int vi_;
  double vd_;
};

using namespace utils::thread;

TEST(ThreadLocal, ThreadLocal) {
  constexpr static int size = 3;
  std::vector<Data*> addrs(size, nullptr);
  std::vector<bool> stop(size, false);
  std::vector<std::thread> thds(size);
  ThreadLocalStorage<Data> tls([]() { return new Data; });

  for (int i = 0; i < size; ++i) {
    std::thread thd([&addrs, &stop, i, tls]() {
      Data* p = tls;
      addrs[i] = p;
      while (!stop[i]) {
        usleep(1000);
      }
    });
    thds[i].swap(thd);
  }

  while (true) {
    if (addrs[0] != nullptr && addrs[1] != nullptr && addrs[2] != nullptr) {
      break;
    }
    usleep(1000);
  }

  for (int i = 0; i < size; ++i) {
    stop[i] = true;
    thds[i].join();
  }

  EXPECT_NE(addrs[0], addrs[1]);
  EXPECT_NE(addrs[2], addrs[1]);
  EXPECT_NE(addrs[0], addrs[2]);
}