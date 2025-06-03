#include "../../include/planning/atom.hpp"

namespace planning {
  Atom::Atom(const int &predicate_id, const std::vector<int> &object_ids)
      : predicate_id(predicate_id), object_ids(object_ids) {}
}  // namespace planning
