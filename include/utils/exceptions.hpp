#ifndef UTILS_EXCEPTIONS_HPP
#define UTILS_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

class NotImplementedError : public std::logic_error {
 public:
  NotImplementedError(const std::string &description)
      : std::logic_error(description + " not yet implemented."){};
};

class NotSupportedError : public std::logic_error {
 public:
  NotSupportedError(const std::string &description)
      : std::logic_error(description + " not supported."){};
};

#endif  // UTILS_EXCEPTIONS_HPP
