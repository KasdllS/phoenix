#include "parser/parser.h"

#include <fstream>
#include <sstream>
#include "parser/parser_impl.h"

namespace parser {

utils::Node Parser::deserialize(const std::string& format,
                                const std::string& bytes) {
  //
  return ParserInst::instance().deserialize(format, bytes);
}

std::string Parser::serialize(const std::string& format,
                              const utils::Node& node) {
  //
  return ParserInst::instance().serialize(format, node);
}

utils::Node Parser::read(const std::string& file) {
  auto pos = file.find_last_of('.');
  if (pos == std::string::npos) {
    return utils::Node();
  }

  auto key = file.substr(pos + 1);
  std::fstream fs(file);
  std::stringstream sstm;
  sstm << fs.rdbuf();
  return parser::ParserInst::instance().deserialize(key, sstm.str());
}

bool Parser::write(const std::string& file, utils::Node& node) {
  auto pos = file.find_last_of('.');
  if (pos == std::string::npos) {
    return false;
  }

  auto key = file.substr(pos + 1);
  auto str = parser::ParserInst::instance().serialize(key, node);
  if (str.empty()) {
    return false;
  }

  std::fstream fs(file, std::ios::in | std::ios::out | std::ios::trunc);
  fs << str;
  fs.close();
  return true;
}

void ParserImpl::registFuncs(const std::string& key,
                             const SerializeFunc& ser,
                             const DeserializeFunc& deser) {
  ser_funcs_[key] = ser;
  deser_funcs_[key] = deser;
}

utils::Node ParserImpl::deserialize(const std::string& key,
                                    const std::string& bytes) {
  if (deser_funcs_.count(key) == 1) {
    return deser_funcs_[key](bytes);
  }
  return utils::Node();
}
std::string ParserImpl::serialize(const std::string& key,
                                  const utils::Node& node) {
  if (ser_funcs_.count(key) == 1) {
    return ser_funcs_[key](node);
  }
  return std::string();
}

}  // namespace parser