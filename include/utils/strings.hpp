#ifndef UTILS_STRINGS
#define UTILS_STRINGS

#include <regex>
#include <string>
#include <vector>

namespace utils {
  std::vector<std::string> tokenise(const std::string &str, const char &delim) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string s;

    while (std::getline(iss, s, delim)) {
      tokens.push_back(s);
    }

    return tokens;
  }

  std::string join(const std::vector<std::string> &tokens, const std::string &delim) {
    std::ostringstream oss;
    for (size_t i = 0; i < tokens.size(); ++i) {
      oss << tokens[i];
      if (i < tokens.size() - 1) {
        oss << delim;
      }
    }
    return oss.str();
  }
}  // namespace utils

#endif  // UTILS_STRINGS
