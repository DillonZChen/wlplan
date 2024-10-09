#include "../../include/planning/fluent.hpp"

namespace planning {
  Fluent::Fluent(const Function &function, const std::vector<Object> &objects)
      : function(std::make_shared<Function>(function)), objects(objects) {}

  std::string Fluent::to_string() const {
    std::string repr = function->name + "(";
    for (size_t i = 0; i < objects.size(); i++) {
      repr += objects[i];
      if (i < objects.size() - 1) {
        repr += ", ";
      }
    }
    repr += ")";
    return repr;
  }
}  // namespace planning
