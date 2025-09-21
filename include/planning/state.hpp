#ifndef PLANNING_STATE_HPP
#define PLANNING_STATE_HPP

#include "atom.hpp"

#include <vector>

namespace wlplan {
  namespace planning {
    class State {
     public:
      std::vector<std::shared_ptr<Atom>> atoms;
      std::vector<double> values;

      State(const std::vector<std::shared_ptr<Atom>> &atoms, const std::vector<double> &values);
      State(const std::vector<std::shared_ptr<Atom>> &atoms);
      State(const std::vector<Atom> &atoms, const std::vector<double> &values);
      State(const std::vector<Atom> &atoms);

      std::vector<Atom> get_atoms() const;
      std::vector<double> get_values() const;

      std::string to_string() const;

      bool operator==(const State &other) const;

      std::size_t hash() const;
    };
  }  // namespace planning
}  // namespace wlplan

#endif  // PLANNING_STATE_HPP
