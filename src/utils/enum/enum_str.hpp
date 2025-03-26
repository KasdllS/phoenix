#pragma once

/**
 * @file enum_str.hpp
 * @author sqi (kasdll@outlook.com)
 * @brief
 * DEFINE_ENUM(Type, Val0 = 0, Val1, Val2)
 * 等于
 * enum class Type {
 *  Begin,
 *  Val0 = 0,
 *  Val1,
 *  Val2,
 *  End,
 * };
 * 提供枚举值范围Begin, End与枚举尺寸 `TypeMap::enum_size` (End - Begin)
 * 提供名称与枚举双射关系："Val0"--Val0, "Val1"--Val1, "Val2"--Val2
 * 与映射接口：
 * TypeMap::name(Val1)-->"Val1"
 * TypeMap::enum("Val1")-->Val1
 *
 * @version 0.1
 * @date 2022-07-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <string_view>
#include "utils/assert.h"
#include "utils/macros.hpp"

#ifndef DEFINE_ENUM
#define DEFINE_ENUM(EnumName, ...)                                     \
  enum class EnumName { Begin, ##__VA_ARGS__, End };                   \
                                                                       \
  class EnumName##Map {                                                \
   public:                                                             \
    constexpr static std::size_t enum_size =                           \
        static_cast<int64_t>(EnumName::End) -                          \
        static_cast<int64_t>(EnumName::Begin);                         \
    EnumName##Map() {                                                  \
      initMap();                                                       \
    }                                                                  \
    static void initMap() {                                            \
      std::string arg = #__VA_ARGS__;                                  \
      std::list<std::string> args = spilit(arg);                       \
      for (auto const& str : args) {                                   \
        assignValue(str);                                              \
      }                                                                \
    }                                                                  \
                                                                       \
    inline static std::map<int, std::string> getMap() {                \
      return enum_str;                                                 \
    }                                                                  \
    inline static std::map<std::string, int> getReverseMap() {         \
      return str_enum;                                                 \
    }                                                                  \
    inline static std::string name(const int idx) {                    \
      if (enum_str.count(idx) == 0) {                                  \
        return "";                                                     \
      }                                                                \
      return enum_str[idx];                                            \
    }                                                                  \
    inline static std::string name(const EnumName val) {               \
      return name(static_cast<int>(val));                              \
    }                                                                  \
    inline static EnumName val(const std::string& name) {              \
      fassert(str_enum.count(name) == 1);                              \
      return static_cast<EnumName>(str_enum[name]);                    \
    }                                                                  \
                                                                       \
   private:                                                            \
    static void removeBlank(std::string& str) {                        \
      auto noSpaceEnd = std::remove(str.begin(), str.end(), ' ');      \
      str.erase(noSpaceEnd, str.end());                                \
    }                                                                  \
                                                                       \
    static std::list<std::string> spilit(const std::string& str) {     \
      std::list<std::string> ret;                                      \
                                                                       \
      std::string source = str;                                        \
      removeBlank(source);                                             \
      std::size_t pos = 0;                                             \
      std::string_view view(source);                                   \
                                                                       \
      while (pos != std::string::npos) {                               \
        pos = view.find(',');                                          \
        if (auto tar = view.substr(0, pos); !tar.empty()) {            \
          ret.emplace_back(tar);                                       \
        }                                                              \
        view = view.substr(pos + 1);                                   \
      }                                                                \
                                                                       \
      return ret;                                                      \
    }                                                                  \
                                                                       \
    static void assignValue(const std::string& str) {                  \
      std::string_view view(str);                                      \
      std::size_t pos = view.find('=');                                \
      auto tar = view.substr(0, pos);                                  \
      if (pos == std::string::npos) {                                  \
        ++value;                                                       \
        enum_str.emplace(value, tar);                                  \
        str_enum.emplace(tar, value);                                  \
      } else {                                                         \
        value = std::atoll(std::string(view.substr(pos + 1)).c_str()); \
        enum_str.emplace(value, tar);                                  \
        str_enum.emplace(tar, value);                                  \
      }                                                                \
    }                                                                  \
                                                                       \
    inline static int value = -1;                                      \
    inline static std::map<int, std::string> enum_str;                 \
    inline static std::map<std::string, int> str_enum;                 \
  };                                                                   \
  inline const static EnumName##Map CONCAT(s_##EnumName, __COUNTER__);
#endif