#include "parser/json.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>

#include "parser/parser.h"

namespace parser {

static utils::Node deserialize(nlohmann::json j) {
  utils::Node n;
  if (j.is_object()) {
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
      n[it.key()] = deserialize(it.value());
    }
  } else if (j.is_array()) {
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
      n.push_back(deserialize(*it));
    }
  } else if (j.is_boolean()) {
    n = j.get<bool>();
  } else if (j.is_number_integer()) {
    n = j.get<int>();
  } else if (j.is_number_float()) {
    n = j.get<double>();
  } else if (j.is_string()) {
    n = j.get<std::string>();
  }
  return n;
}

utils::Node Json::deserialize(const std::string& bytes) {
  auto j = nlohmann::json::parse(bytes);
  return ::parser::deserialize(j);
}

static nlohmann::json serialize(const utils::Node& node) {
  nlohmann::json j;
  if (node.isMap()) {
    for (auto iter = node.begin(); iter != node.end(); ++iter) {
      j[iter->first] = serialize(iter->second);
    }
  } else if (node.isArray()) {
    for (auto iter = node.begin(); iter != node.end(); ++iter) {
      j.push_back(serialize(*iter));
    }
  } else if (node.isBool()) {
    j = node.as<bool>();
  } else if (node.isInteger()) {
    j = node.as<int>();
  } else if (node.isDouble()) {
    j = node.as<double>();
  } else if (node.isString()) {
    j = node.as<std::string>();
  }
  return j;
}

std::string Json::serialize(const utils::Node& node) {
  std::stringstream sstm;
  sstm << ::parser::serialize(node);
  return sstm.str();
}

REGIST_PARSER(Json)

}  // namespace parser