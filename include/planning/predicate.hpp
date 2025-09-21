#ifndef PLANNING_PREDICATE_HPP
#define PLANNING_PREDICATE_HPP

#include <string>

namespace wlplan {
  namespace planning {
    class Predicate {
     public:
      std::string name;
      int arity;

      Predicate() = default;

      Predicate(const std::string &name, const int arity);

      std::string to_string() const;

      bool operator==(const Predicate &other) const { return to_string() == other.to_string(); }

      bool operator<(const Predicate &other) const { return to_string() < other.to_string(); }
    };

  }  // namespace planning
}  // namespace wlplan

template <>
class std::hash<wlplan::planning::Predicate> {
  std::size_t operator()(const wlplan::planning::Predicate &k) const {
    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:

    return (std::hash<std::string>()(k.name) ^ (std::hash<int>()(k.arity) << 1)) >> 1;
  }
};

#endif  // PLANNING_PREDICATE_HPP
