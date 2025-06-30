#include "../../include/planning/action.hpp"

namespace planning {
  Action::Action(const Schema &schema, const std::vector<Object> &objects)
      : schema(std::make_shared<Schema>(schema)), objects(objects) {}

  std::string Action::to_string() const {
    std::string repr = schema->name + "(";
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
