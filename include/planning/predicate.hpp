#ifndef PLANNING_PREDICATE_HPP
#define PLANNING_PREDICATE_HPP

#include <string>

namespace planning {
  class Predicate {
   public:
    std::string name;
    int arity;

    Predicate(const std::string &name, const int arity);

    std::string to_string() const;

    bool operator==(const Predicate &other) const { return to_string() == other.to_string(); }

    bool operator<(const Predicate &other) const { return to_string() < other.to_string(); }
  };

}  // namespace planning

#endif  // PLANNING_PREDICATE_HPP
