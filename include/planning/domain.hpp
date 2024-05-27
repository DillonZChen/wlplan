#ifndef PLANNING_DOMAIN_HPP
#define PLANNING_DOMAIN_HPP

#include "predicate.hpp"
#include "state.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace py = pybind11;

namespace planning {
  class Domain {
   public:
    const std::string name;
    const std::vector<Predicate> predicates;
    const std::vector<Object> constant_objects;
    std::unordered_map<std::string, int> predicate_to_colour;

    Domain(const std::string &name,
           const std::vector<Predicate> &predicates,
           const std::vector<Object> &constant_objects);

    Domain(const std::string &name, const std::vector<Predicate> &predicates);

    int max_arity() const;

    std::string to_string() const;

    bool operator==(const Domain &other) const {
      return name == other.name && predicates == other.predicates &&
             constant_objects == other.constant_objects;
    }
  };
}  // namespace planning

#endif  // PLANNING_DOMAIN_HPP
