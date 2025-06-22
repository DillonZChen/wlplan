#include "../../include/planning/state_verbose.hpp"

#include <algorithm>

namespace planning {
  StateVerbose::StateVerbose(const std::vector<std::shared_ptr<planning::AtomVerbose>> &atoms,
               const std::vector<double> &values)
      : atoms(atoms), values(values) {}

  StateVerbose::StateVerbose(const std::vector<std::shared_ptr<planning::AtomVerbose>> &atoms) : atoms(atoms) {}

  StateVerbose::StateVerbose(const std::vector<planning::AtomVerbose> &atoms) {
    for (const planning::AtomVerbose &atom : atoms) {
      this->atoms.push_back(std::make_shared<planning::AtomVerbose>(atom));
    }
  }

  StateVerbose::StateVerbose(const std::vector<planning::AtomVerbose> &atoms, const std::vector<double> &values) {
    for (const planning::AtomVerbose &atom : atoms) {
      this->atoms.push_back(std::make_shared<planning::AtomVerbose>(atom));
    }
    this->values = values;
  }

  std::vector<planning::AtomVerbose> StateVerbose::get_atoms() const {
    std::vector<planning::AtomVerbose> ret;
    for (const std::shared_ptr<planning::AtomVerbose> &atom : atoms) {
      ret.push_back(*atom);
    }
    return ret;
  }

//   std::string StateVerbose::to_string() const {
//     std::string ret = "StateVerbose(atoms=[";

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

  bool StateVerbose::operator==(const StateVerbose &other) const {
    return atoms == other.atoms && values == other.values;
  }

//   size_t StateVerbose::hash() const { return std::hash<std::string>()(to_string()); }
}  // namespace planning
