#include "../../include/planning/atom_verbose.hpp"

namespace planning {
  AtomVerbose::AtomVerbose(const Predicate &predicate, const std::vector<std::string> &objects)
      : predicate(predicate), objects(objects) {}
}  // namespace planning
