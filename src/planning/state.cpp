#include "../../include/planning/state.hpp"

#include <algorithm>

namespace planning {
  State::State(const std::vector<std::shared_ptr<planning::Atom>> &atoms,
               const std::vector<double> &values)
      : atoms(atoms), values(values) {}

  State::State(const std::vector<std::shared_ptr<planning::Atom>> &atoms) : atoms(atoms) {}

  State::State(const std::vector<planning::Atom> &atoms) {
    for (const planning::Atom &atom : atoms) {
      this->atoms.push_back(std::make_shared<planning::Atom>(atom));
    }
  }

  State::State(const std::vector<planning::Atom> &atoms, const std::vector<double> &values) {
    for (const planning::Atom &atom : atoms) {
      this->atoms.push_back(std::make_shared<planning::Atom>(atom));
    }
    this->values = values;
  }

  std::vector<planning::Atom> State::get_atoms() const {
    std::vector<planning::Atom> ret;
    for (const std::shared_ptr<planning::Atom> &atom : atoms) {
      ret.push_back(*atom);
    }
    return ret;
  }

//   std::string State::to_string() const {
//     std::string ret = "State(atoms=[";

//     // sort atoms because order does not matter
//     std::vector<std::string> atom_strings;
//     for (size_t i = 0; i < atoms.size(); i++) {
//       atom_strings.push_back(atoms[i]->to_string());
//     }
//     std::sort(atom_strings.begin(), atom_strings.end());
//     for (size_t i = 0; i < atom_strings.size(); i++) {
//       ret += atom_strings[i];
//       if (i < atom_strings.size() - 1) {
//         ret += ", ";
//       }
//     }
//     ret += "], values=[";

//     // do not sort values because order depends on fluent order
//     for (size_t i = 0; i < values.size(); i++) {
//       ret += std::to_string(values[i]);
//       if (i < values.size() - 1) {
//         ret += ", ";
//       }
//     }
//     ret += "])";
//     return ret;
//   }

  bool State::operator==(const State &other) const {
    return atoms == other.atoms && values == other.values;
  }

//   size_t State::hash() const { return std::hash<std::string>()(to_string()); }
}  // namespace planning
