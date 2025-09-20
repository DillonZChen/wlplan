#include "../../include/planning/schema.hpp"

#include <stdexcept>

namespace wlplan {
  namespace planning {
    Schema::Schema(const std::string &name, const int arity) : name(name), arity(arity) {
      if (arity < 0) {
        std::string err_msg = "Schema " + name + " has arity " + std::to_string(arity) + " < 0";
        throw std::invalid_argument(err_msg);
      }
    }

    std::string Schema::to_string() const { return name + "/" + std::to_string(arity); }
  }  // namespace planning
}  // namespace wlplan
