#include <gtest/gtest.h>
#include <iostream>
#include "utils/meta.hpp"

using namespace utils;

void test_const(const Node& node, const std::string& tar) {
  EXPECT_EQ(node[tar].as<bool>(), false);
}

void test_const(const Node& node, const int idx) {
  EXPECT_EQ(node[idx].as<bool>(), false);
}

TEST(Meta, Meta) {
  Node n;
  n = true;
  EXPECT_EQ(n.isBool(), true);
  EXPECT_EQ(n.isInteger(), false);
  EXPECT_EQ(n.as<bool>(), true);
  n = 1;
  EXPECT_EQ(n.isInteger(), true);
  EXPECT_EQ(n.isDouble(), false);
  EXPECT_EQ(n.as<int>(), 1);
  n = 1.0;
  EXPECT_EQ(n.isDouble(), true);
  EXPECT_EQ(n.isInteger(), false);
  EXPECT_EQ(n.as<double>(), 1.0);
  n = "hello";
  EXPECT_EQ(n.isString(), true);
  EXPECT_EQ(n.isBool(), false);
  EXPECT_EQ(n.as<std::string>(), "hello");
}

TEST(Meta, Array) {
  Node n;
  n.push_back(true);
  n.push_back(10);
  n.push_back(-1.0);
  n.push_back("hello");

  EXPECT_EQ(n.isArray(), true);

  int i = 0;
  for (auto iter = n.begin(); iter != n.end(); ++iter) {
    switch (i++) {
      case 0:
        EXPECT_EQ((*iter).isBool(), true);
        EXPECT_EQ((*iter).as<bool>(), true);
        break;

      case 1:
        EXPECT_EQ((*iter).isInteger(), true);
        EXPECT_EQ((*iter).as<int>(), 10);
        break;

      case 2:
        EXPECT_EQ((*iter).isDouble(), true);
        EXPECT_EQ((*iter).as<double>(), -1.0);
        break;

      case 3:
        EXPECT_EQ((*iter).isString(), true);
        EXPECT_EQ((*iter).as<std::string>(), "hello");
        break;

      default:
        break;
    }
  }
  EXPECT_EQ(i, 4);

  i = 0;
  for (auto iter = n.begin(); iter != n.end(); ++iter) {
    switch (i++) {
      case 0:
        EXPECT_EQ((*iter).isBool(), true);
        EXPECT_EQ((*iter).as<bool>(), true);
        break;

      case 1:
        EXPECT_EQ((*iter).isInteger(), true);
        EXPECT_EQ((*iter).as<int>(), 10);
        break;

      case 2:
        EXPECT_EQ((*iter).isDouble(), true);
        EXPECT_EQ((*iter).as<double>(), -1.0);
        break;

      case 3:
        EXPECT_EQ((*iter).isString(), true);
        EXPECT_EQ((*iter).as<std::string>(), "hello");
        break;

      default:
        break;
    }
  }
  EXPECT_EQ(i, 4);

  n.clear();
  n.push_back(0.0);
  n.push_back(1.2);
  n.push_back(3);
  n.push_back("hello");
  n.push_back(true);

  auto vec = n.as<std::list<int>>();
  EXPECT_EQ(vec.size(), 4);
}

TEST(Meta, Map) {
  Node n;
  n.insert("1", true);
  n.insert("2", 1);
  n.insert("3", 100.0);
  n.insert("4", "hello");

  EXPECT_EQ(n.hasMember("2"), true);

  for (auto iter = n.begin(); iter != n.end(); ++iter) {
    if (iter->first == "1") {
      EXPECT_EQ(iter->second.isBool(), true);
    } else if (iter->first == "2") {
      EXPECT_EQ(iter->second.isInteger(), true);
    } else if (iter->first == "3") {
      EXPECT_EQ(iter->second.isDouble(), true);
    } else if (iter->first == "4") {
      EXPECT_EQ(iter->second.isString(), true);
    }
  }

  for (auto iter = n.begin(); iter != n.end(); ++iter) {
    if (iter->first == "1") {
      EXPECT_EQ(iter->second.isBool(), true);
    } else if (iter->first == "2") {
      EXPECT_EQ(iter->second.isInteger(), true);
    } else if (iter->first == "3") {
      EXPECT_EQ(iter->second.isDouble(), true);
    } else if (iter->first == "4") {
      EXPECT_EQ(iter->second.isString(), true);
    }
  }

  n["1"] = 3.1;
  EXPECT_EQ(n["1"].as<double>(), 3.1);

  auto ret = n.as<std::unordered_map<std::string, double>>();
  EXPECT_EQ(ret.size(), 3);
}

TEST(Meta, Assign) {
  Node n;
  n.push_back(1.0);
  n.push_back(2);
  n.push_back("hello");

  Node other;
  other = n;
  EXPECT_EQ(other.isArray(), true);
  EXPECT_EQ(other.size(), 3);

  n.clear();

  /**
   *
   * {
   *  "loader": {
   *      "boom_length": 2.93,
   *      "bucket_length": 1.2,
   *      "move_motor": {
   *          "torque_range": [-1500, 3000]
   *          "speed_range": [[0, 1500], [0, 3000]]
   *      }
   *  }
   * }
   *
   */
  n["loader"]["boom_length"] = 2.93;
  n["loader"]["bucket_length"] = 1.2;
  n["loader"]["move_motor"]["torque_range"].push_back(-1500);
  n["loader"]["move_motor"]["torque_range"].push_back(3000);

  Node range;
  range.push_back(0);
  range.push_back(1500);
  n["loader"]["move_motor"]["speed_range"].push_back(range.clone());

  range.clear();
  range.push_back(0);
  range.push_back(3000);
  n["loader"]["move_motor"]["speed_range"].push_back(range.clone());

  EXPECT_EQ(n.hasMember("loader"), true);
  EXPECT_EQ(n["loader"].size(), 3);
  EXPECT_EQ(n["loader"]["move_motor"].size(), 2);
}

TEST(Meta, Copy) {
  Node n;
  n = 1.0;
  Node other = n;
  other = 2;
  EXPECT_EQ(n.isInteger(), true);
  EXPECT_EQ(n.as<int>(), 2);

  Node tmp;
  tmp = false;
  *other = *tmp;
  EXPECT_EQ(n.isBool(), true);
  EXPECT_EQ(n.isInteger(), false);
  EXPECT_EQ(n.as<bool>(), false);

  other = n.clone();
  other = "hello";

  EXPECT_EQ(n.isBool(), true);
  EXPECT_EQ(n.as<bool>(), false);
  EXPECT_EQ(other.isString(), true);
}

TEST(Meta, Merge) {
  Node n1;
  n1["test1"]["test2"]["test3"].push_back(1);
  n1["test1"]["test2"]["test3"].push_back(2);
  n1["test1"]["test2"]["test3"].push_back(3);

  Node n2;
  n2["test1"]["test4"] = 2.3;
  n1.merge(n2);
  EXPECT_EQ(n1["test1"].hasMember("test4"), true);

  n1.merge(n2, "/test1/test2");
  EXPECT_EQ(n1["test1"]["test2"].hasMember("test1"), true);

  EXPECT_EQ(n1["test1"]["test2"]["test1"]["test4"].as<double>(), 2.3);
  n1["test1"]["test2"]["test1"]["test4"] = "hello world";
  EXPECT_EQ(n1["test1"]["test2"]["test1"]["test4"].as<std::string>(),
            "hello world");
  EXPECT_EQ(n2["test1"]["test4"].as<double>(), 2.3);
  n1.merge(n2, "/test1/test3");
  EXPECT_EQ(n1["test1"].hasMember("test3"), false);
}

TEST(Meta, Const) {
  Node n;
  n["test"] = false;
  test_const(n, "test");
  n.push_back(false);
  test_const(n, 0);
}

TEST(Meta, As) {
  Node n;
  n = "73";
  EXPECT_EQ(n.as<int>(), 73);
  EXPECT_EQ(n.as<bool>(), true);
  EXPECT_EQ(n.as<double>(), static_cast<double>(73));
}