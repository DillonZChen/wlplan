#ifndef PLANNING_ATOM_HPP
#define PLANNING_ATOM_HPP

#include "object.hpp"
#include "predicate.hpp"

#include <memory>
#include <string>
#include <vector>

namespace planning {
  class Atom {
   public:
    const int predicate_id;
    const std::vector<int> object_ids;

    Atom(const int &predicate_id, const std::vector<int> &object_ids);

    bool operator==(const Atom &other) const {
      return predicate_id == other.predicate_id && object_ids == other.object_ids;
    }
  };

}  // namespace planning

#endif  // PLANNING_ATOM_HPP
