#ifndef UTILS_EXCEPTIONS_HPP
#define UTILS_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

class NotImplementedException : public std::logic_error {
 public:
  NotImplementedException(const std::string &description)
      : std::logic_error(description + " not yet implemented."){};
};

#endif  // UTILS_EXCEPTIONS_HPP
