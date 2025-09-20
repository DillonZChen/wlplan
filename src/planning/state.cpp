#include "../../include/planning/state.hpp"

#include <algorithm>

namespace wlplan {
  namespace planning {
    State::State(const std::vector<std::shared_ptr<Atom>> &atoms, const std::vector<double> &values)
        : atoms(atoms), values(values) {}

    State::State(const std::vector<std::shared_ptr<Atom>> &atoms) : atoms(atoms) {}

    State::State(const std::vector<Atom> &atoms) {
      for (const Atom &atom : atoms) {
        this->atoms.push_back(std::make_shared<Atom>(atom));
      }
    }

    State::State(const std::vector<Atom> &atoms, const std::vector<double> &values) {
      for (const Atom &atom : atoms) {
        this->atoms.push_back(std::make_shared<Atom>(atom));
      }
      this->values = values;
    }

    std::vector<Atom> State::get_atoms() const {
      std::vector<Atom> ret;
      for (const std::shared_ptr<Atom> &atom : atoms) {
        ret.push_back(*atom);
      }
      return ret;
    }

    std::vector<double> State::get_values() const { return values; }

    std::string State::to_string() const {
      std::string ret = "State(atoms=[";

      // sort atoms because order does not matter
      std::vector<std::string> atom_strings;
      for (size_t i = 0; i < atoms.size(); i++) {
        atom_strings.push_back(atoms[i]->to_string());
      }
      std::sort(atom_strings.begin(), atom_strings.end());
      for (size_t i = 0; i < atom_strings.size(); i++) {
        ret += atom_strings[i];
        if (i < atom_strings.size() - 1) {
          ret += ", ";
        }
      }
      ret += "], values=[";

      // do not sort values because order depends on fluent order
      for (size_t i = 0; i < values.size(); i++) {
        ret += std::to_string(values[i]);
        if (i < values.size() - 1) {
          ret += ", ";
        }
      }
      ret += "])";
      return ret;
    }

    bool State::operator==(const State &other) const {
      return atoms == other.atoms && values == other.values;
    }

    size_t State::hash() const { return std::hash<std::string>()(to_string()); }
  }  // namespace planning
}  // namespace wlplan
