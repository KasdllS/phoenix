#include "utils/data_sync.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(DataSync, Vector)
{
    DataSync<std::vector<int>, std::mutex, std::lock_guard<std::mutex>> datas;
    datas.push_back(1);
    EXPECT_EQ(datas.size(), 1);

    std::vector<int> test = {1, 2, 3};
    datas = test;
    EXPECT_EQ(datas.size(), 3);
    auto res = datas.get();
    EXPECT_EQ(res[2], 3);
    datas.clear();
    EXPECT_EQ(datas.size(), 0);
}