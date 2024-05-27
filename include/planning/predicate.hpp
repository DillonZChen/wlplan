#ifndef PLANNING_PREDICATE_HPP
#define PLANNING_PREDICATE_HPP

#include <string>

namespace planning {
  class Predicate {
   public:
    const std::string name;
    const int arity;

    Predicate(const std::string &name, int arity);

    std::string to_string() const;

    bool operator==(const Predicate &other) const {
      return name == other.name && arity == other.arity;
    }
  };

}  // namespace planning

#endif  // PLANNING_PREDICATE_HPP
