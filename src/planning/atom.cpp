#include "../../include/planning/atom.hpp"

namespace wlplan {
  namespace planning {
    Atom::Atom(const Predicate &predicate, const std::vector<Object> &objects)
        : predicate(std::make_shared<Predicate>(predicate)), objects(objects) {}

    std::string Atom::to_pddl() const {
      std::string repr = "(" + predicate->name;
      for (size_t i = 0; i < objects.size(); i++) {
        repr += " " + objects[i];
      }
      repr += ")";
      return repr;
    }

    std::string Atom::to_string() const {
      std::string repr = predicate->name + "(";
      for (size_t i = 0; i < objects.size(); i++) {
        repr += objects[i] + ",";
      }
      repr += ")";
      return repr;
    }
  }  // namespace planning
}  // namespace wlplan
