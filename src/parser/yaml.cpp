#include "parser/yaml.h"

#include <yaml-cpp/yaml.h>
#include "parser/parser.h"

namespace parser {

static utils::Node scalar2node(YAML::Node yml) {
  utils::Node n;
  try {
    n = yml.as<bool>();
    return n;
  } catch (std::exception& e) {
  }

  try {
    n = yml.as<int>();
    return n;
  } catch (std::exception& e) {
  }

  try {
    n = yml.as<double>();
    return n;
  } catch (std::exception& e) {
  }

  try {
    n = yml.as<std::string>();
  } catch (std::exception& e) {
  }

  return n;
}

static utils::Node deserialize(YAML::Node yml) {
  utils::Node n;
  if (yml.IsMap()) {
    for (auto it = yml.begin(); it != yml.end(); ++it) {
      n[it->first.as<std::string>()] = deserialize(it->second);
    }
  } else if (yml.IsSequence()) {
    for (auto it = yml.begin(); it != yml.end(); ++it) {
      n.push_back(deserialize(*it));
    }
  } else if (yml.IsScalar()) {
    n = scalar2node(yml);
  }

  return n;
}

utils::Node Yaml::deserialize(const std::string& bytes) {
  return ::parser::deserialize(YAML::Load(bytes));
}

static YAML::Node serialize(const utils::Node& node) {
  YAML::Node n;
  if (node.isMap()) {
    for (auto iter = node.begin(); iter != node.end(); ++iter) {
      n[iter->first] = serialize(iter->second);
    }
  } else if (node.isArray()) {
    for (auto iter = node.begin(); iter != node.end(); ++iter) {
      n.push_back(serialize(*iter));
    }
    n.SetStyle(YAML::EmitterStyle::Flow);
  } else if (node.isBool()) {
    n = node.as<bool>();
  } else if (node.isInteger()) {
    n = node.as<int>();
  } else if (node.isDouble()) {
    n = node.as<double>();
  } else if (node.isString()) {
    n = node.as<std::string>();
  }
  return n;
}

std::string Yaml::serialize(const utils::Node& node) {
  std::stringstream sstm;
  sstm << ::parser::serialize(node);
  return sstm.str();
}

REGIST_PARSER(Yaml)

}  // namespace parser