#include "utils/meta/node.h"

#include <gtest/gtest.h>

using namespace phoenix;

TEST(Node, AssignBool) {
  Node n(false);
  EXPECT_EQ(n.as<bool>(), false);
  n = true;
  EXPECT_EQ(n.as<bool>(), true);
  EXPECT_EQ(n.isValue(), true);
}

TEST(Node, AssignInt) {
  Node n(-10);
  EXPECT_EQ(n.as<int>(), -10);
  n = 100;
  EXPECT_EQ(n.as<int>(), 100);
  EXPECT_EQ(n.isValue(), true);
}

TEST(Node, AssignFloat) {
  Node n(-1.234f);
  EXPECT_EQ(n.as<float>(), -1.234f);
  n = 10.22333;
  EXPECT_EQ(n.as<double>(), 10.22333);
  EXPECT_EQ(n.isValue(), true);
}

TEST(Node, AssignString) {
  Node n("hello");
  EXPECT_EQ(n.as<std::string>(), "hello");
  n = 10.22333;
  EXPECT_EQ(n.as<double>(), 10.22333);
  n = "world";
  EXPECT_EQ(n.as<std::string>(), "world");
  EXPECT_EQ(n.isValue(), true);
}

TEST(Node, AssignMap) {
  Node n;
  std::map<std::string, int, std::less<>> nodes;
  nodes["test"] = 100;
  n = nodes;
  n["hello1"] = 1.2f;
  EXPECT_EQ(n["test"].as<int>(), 100);
  EXPECT_EQ(n["hello1"].as<float>(), 1.2f);
  EXPECT_EQ(n.isMap(), true);
}

TEST(Node, AssignVector) {
  Node n;
  std::vector<double> vecs(2);
  vecs[0] = 1;
  vecs[1] = -100;
  n = vecs;
  EXPECT_EQ(n[1].as<double>(), -100);
  EXPECT_EQ(n.isVector(), true);
}

TEST(Node, Child) {
  Node n;
  std::map<std::string, Node, std::less<>> nodes, child;
  nodes["test"] = 100;
  nodes["hello1"] = 1.2f;
  child["test/test"] = false;
  nodes["child"] = child;
  n = nodes;

  EXPECT_EQ(n["test"].as<int>(), 100);
  EXPECT_EQ(n["hello1"].as<float>(), 1.2f);
  EXPECT_EQ(n["child"]["test/test"].as<bool>(), false);
  EXPECT_EQ(n.isMap(), true);
  EXPECT_EQ(n["test"].isValue(), true);
  EXPECT_EQ(n["child"].isMap(), true);

  std::vector<Node> vec(3);
  vec[0] = 1;
  vec[1] = "hello";
  vec[2] = child;

  n = vec;

  EXPECT_EQ(n[0].as<int>(), 1);
  EXPECT_EQ(n[1].as<std::string>(), "hello");
  EXPECT_EQ(n[2]["test/test"].as<bool>(), false);
  EXPECT_EQ(n.isVector(), true);
  EXPECT_EQ(n[2].isMap(), true);
}