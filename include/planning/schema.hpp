#ifndef PLANNING_SCHEMA_HPP
#define PLANNING_SCHEMA_HPP

#include <string>

namespace planning {
  class Schema {
   public:
    std::string name;
    int arity;

    Schema() = default;

    Schema(const std::string &name, const int arity);

    std::string to_string() const;

    bool operator==(const Schema &other) const { return to_string() == other.to_string(); }

    bool operator<(const Schema &other) const { return to_string() < other.to_string(); }
  };

}  // namespace planning

template <> class std::hash<planning::Schema> {
  std::size_t operator()(const planning::Schema &k) const {
    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:

    return (std::hash<std::string>()(k.name) ^ (std::hash<int>()(k.arity) << 1)) >> 1;
  }
};

#endif  // PLANNING_SCHEMA_HPP
