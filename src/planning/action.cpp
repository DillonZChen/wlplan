#include "../../include/planning/action.hpp"

namespace planning {
  Action::Action(const Schema &schema, const std::vector<Object> &objects)
      : schema(std::make_shared<Schema>(schema)), objects(objects) {}

  py::tuple Action::__getstate__(const Action &input) {
    return py::make_tuple(*(input.schema), input.objects);
  }

  Action Action::__setstate__(py::tuple t) {
    if (t.size() != 2) {
      throw std::runtime_error("Invalid state for Action: expected 2 elements, got " +
                               std::to_string(t.size()));
    }
    return Action(t[0].cast<planning::Schema>(), t[1].cast<std ::vector<Object>>());
  }

  std::string Action::to_pddl() const {
    std::string repr = "(" + schema->name;
    for (size_t i = 0; i < objects.size(); i++) {
      repr += " " + objects[i];
    }
    repr += ")";
    return repr;
  }

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
