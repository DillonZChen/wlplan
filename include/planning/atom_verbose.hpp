#ifndef PLANNING_ATOM_VERBOSE_HPP
#define PLANNING_ATOM_VERBOSE_HPP

#include "object.hpp"
#include "predicate.hpp"

#include <memory>
#include <string>
#include <vector>

namespace planning {
  class AtomVerbose {
   public:
    const Predicate predicate;
    const std::vector<std::string> objects;

    AtomVerbose(const Predicate &predicate, const std::vector<std::string> &objects);

    bool operator==(const AtomVerbose &other) const {
      return predicate == other.predicate && objects == other.objects;
    }
  };

}  // namespace planning

#endif  // PLANNING_ATOM_VERBOSE_HPP
