#include "../../include/planning/function.hpp"

#include <stdexcept>

namespace wlplan {
  namespace planning {
    Function::Function(const std::string &name, const int arity) : name(name), arity(arity) {
      if (arity < 0) {
        std::string err_msg = "Function " + name + " has arity " + std::to_string(arity) + " < 0";
        throw std::invalid_argument(err_msg);
      }
    }

    std::string Function::to_string() const { return name + "/" + std::to_string(arity); }
  }  // namespace planning
}  // namespace wlplan
