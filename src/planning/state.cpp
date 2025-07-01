#include "../../include/planning/state.hpp"

#include <algorithm>

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

  py::tuple State::__getstate__(const planning::State &input) {
    std::vector<Atom> atoms = input.get_atoms();
    std::vector<double> values = input.get_values();
    return py::make_tuple(atoms, values);
  }

  State State::__setstate__(py::tuple t) {
    if (t.size() != 2) {
      throw std::runtime_error("Invalid state for State: expected 2 elements, got " +
                               std::to_string(t.size()));
    }
    return State(t[0].cast<std::vector<Atom>>(), t[1].cast<std::vector<double>>());
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
