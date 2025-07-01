#include "../../include/planning/atom.hpp"

namespace planning {
  Atom::Atom(const Predicate &predicate, const std::vector<Object> &objects)
      : predicate(std::make_shared<Predicate>(predicate)), objects(objects) {}

  py::tuple Atom::__getstate__(const Atom &input) {
    return py::make_tuple(*(input.predicate), input.objects);
  }

  Atom Atom::__setstate__(py::tuple t) {
    if (t.size() != 2) {
      throw std::runtime_error("Invalid state for Atom: expected 2 elements, got " +
                               std::to_string(t.size()));
    }
    return Atom(t[0].cast<planning::Predicate>(), t[1].cast<std::vector<Object>>());
  }

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
      repr += objects[i];
      if (i < objects.size() - 1) {
        repr += ", ";
      }
    }
    repr += ")";
    return repr;
  }
}  // namespace planning
