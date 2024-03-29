#include "utils/meta/node.hpp"

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

TEST(Node, AsMeta) {
  Node n;
  n = 1.23f;
  auto meta = n.as<MetaImpl<float>>();
  EXPECT_EQ(meta.val_, 1.23f);
}

TEST(Node, MakeBool) {
  auto n = makeNode<bool>(false, false);
  EXPECT_EQ(n.as<bool>(), false);
  n = true;
  EXPECT_EQ(n.as<bool>(), true);

  bool b = false;
  n = makeNode<bool&>(b, false);
  b = true;
  EXPECT_EQ(n.as<bool&>(), true);
}

TEST(Node, Range) {
  auto n = makeNode<float>(1.23f, 0.123f, 0.224f, 0.222f);
  auto meta = n.as<MetaImpl<float>>();
  EXPECT_EQ(meta.min_, 0.123f);
  EXPECT_EQ(meta.max_, 0.224f);
  EXPECT_EQ(meta.step_, 0.222f);
}

TEST(Node, Clone) {
  Node n, oth;
  n = 1.23f;
  oth = n.clone();
  oth = 2.34f;
  auto meta = n.as<MetaImpl<float>>();
  EXPECT_EQ(meta.val_, 1.23f);
  oth = n;
  oth = 2.34f;
  meta = n.as<MetaImpl<float>>();
  EXPECT_EQ(meta.val_, 2.34f);
}

TEST(Node, NodeChild) {
  Node n;
  Node c = n.child();
  c = 1.23f;
  EXPECT_EQ(n.child().as<float>(), 1.23f);
  Node oth = n.clone();
  oth.child() = 2.24f;
  EXPECT_EQ(n.child().as<float>(), 1.23f);
  EXPECT_EQ(oth.child().as<float>(), 2.24f);
}