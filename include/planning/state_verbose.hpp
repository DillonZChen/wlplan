#ifndef PLANNING_STATE_VERBOSE_HPP
#define PLANNING_STATE_VERBOSE_HPP

#include "atom_verbose.hpp"

#include <vector>

namespace planning {
  class StateVerbose {
   public:
    std::vector<std::shared_ptr<planning::AtomVerbose>> atoms;
    std::vector<double> values;

    StateVerbose(const std::vector<std::shared_ptr<planning::AtomVerbose>> &atoms,
          const std::vector<double> &values);
    StateVerbose(const std::vector<std::shared_ptr<planning::AtomVerbose>> &atoms);
    StateVerbose(const std::vector<planning::AtomVerbose> &atoms, const std::vector<double> &values);
    StateVerbose(const std::vector<planning::AtomVerbose> &atoms);

    // for Python bindings
    std::vector<planning::AtomVerbose> get_atoms() const;

    // std::string to_string() const;

    bool operator==(const StateVerbose &other) const;

    // std::size_t hash() const;
  };
}  // namespace planning

#endif  // PLANNING_STATE_VERBOSE_HPP
