#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace utils::strings {

/**
 * @brief 按目标串切分的函数，切分结果去空
 * input:
 * str:/test/test2//test3 tar:"/"
 * return:
 * {test, test2, test3}
 *
 * @param str
 * @param tar
 * @return std::vector<std::string_view>
 */
inline std::vector<std::string_view> split(const std::string_view& str,
                                           const std::string& tar) {
  std::vector<std::string_view> ret;
  ret.reserve(str.size());

  std::size_t beg = 0;
  std::size_t end = 0;
  while (end != std::string::npos) {
    end = str.find(tar, beg);
    if (auto sub = str.substr(beg, end - beg); !sub.empty()) {
      ret.emplace_back(sub);
    }
    beg = end + tar.size();
  }

  ret.shrink_to_fit();

  return ret;
}

/**
 * @brief
 * 用tar字串连接str中的每一个元素
 * input:
 * str:{test, test2, test3}
 * tar:"/"
 * return:
 * /test/test2/test3/
 *
 * @param str
 * @param tar
 * @return std::string
 */
inline std::string concat(const std::vector<std::string_view>& str,
                          const std::string& tar) {
  std::string ret;
  for (auto const& s : str) {
    ret += tar + std::string(s);
  }
  if (!str.empty()) {
    ret += tar;
  }
  return ret;
}

}  // namespace utils::strings