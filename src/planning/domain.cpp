#include "../../include/planning/domain.hpp"

#include <algorithm>

namespace planning {
  Domain::Domain(const std::string &name,
                 const std::vector<Predicate> &predicates,
                 const std::vector<Function> &functions,
                 const std::vector<Object> &constant_objects)
      : name(name),
        predicates(predicates),
        functions(functions),
        constant_objects(constant_objects) {
    // sort items to ensure consistency when saving and loading models
    std::sort(this->functions.begin(), this->functions.end());
    std::sort(this->predicates.begin(), this->predicates.end());
    std::sort(this->constant_objects.begin(), this->constant_objects.end());
    predicate_to_colour = std::unordered_map<std::string, int>();
    for (size_t i = 0; i < this->predicates.size(); i++) {
      predicate_to_colour[predicates[i].name] = i;
    }
  }

  Domain::Domain(const std::string &name,
                 const std::vector<Predicate> &predicates,
                 const std::vector<Function> &functions)
      : Domain(name, predicates, functions, std::vector<Object>()) {}

  Domain::Domain(const std::string &name,
                 const std::vector<Predicate> &predicates,
                 const std::vector<Object> &constant_objects)
      : Domain(name, predicates, std::vector<Function>(), constant_objects) {}

  Domain::Domain(const std::string &name, const std::vector<Predicate> &predicates)
      : Domain(name, predicates, std::vector<Function>(), std::vector<Object>()) {}

  std::unordered_map<std::string, Predicate> Domain::get_name_to_predicate() const {
    std::unordered_map<std::string, Predicate> name_to_predicate;
    for (const auto &pred : predicates) {
      name_to_predicate[pred.name] = pred;
    }
    return name_to_predicate;
  }

  std::unordered_map<std::string, Function> Domain::get_name_to_function() const {
    std::unordered_map<std::string, Function> name_to_function;
    for (const auto &func : functions) {
      name_to_function[func.name] = func;
    }
    return name_to_function;
  }

  int Domain::get_max_arity() const {
    int max_arity = 0;
    for (size_t i = 0; i < functions.size(); i++) {
      max_arity = std::max(max_arity, functions[i].arity);
    }
    for (size_t i = 0; i < predicates.size(); i++) {
      max_arity = std::max(max_arity, predicates[i].arity);
    }
    return max_arity;
  }

  json Domain::to_json() const {
    json j;
    j["name"] = name;
    std::vector<std::pair<std::string, int>> predicates_raw;
    std::vector<std::pair<std::string, int>> functions_raw;
    for (size_t i = 0; i < predicates.size(); i++) {
      predicates_raw.push_back(std::make_pair(predicates[i].name, predicates[i].arity));
    }
    for (size_t i = 0; i < functions.size(); i++) {
      functions_raw.push_back(std::make_pair(functions[i].name, functions[i].arity));
    }
    j["predicates"] = predicates_raw;
    j["functions"] = functions_raw;
    j["constant_objects"] = constant_objects;
    return j;
  }

  std::string Domain::to_string() const {
    std::string repr = "wlplan.Domain(name=" + name + ", predicates=[";
    for (size_t i = 0; i < predicates.size(); i++) {
      repr += predicates[i].to_string();
      if (i < predicates.size() - 1) {
        repr += ", ";
      }
    }
    repr += "], functions=[";
    for (size_t i = 0; i < functions.size(); i++) {
      repr += functions[i].to_string();
      if (i < functions.size() - 1) {
        repr += ", ";
      }
    }
    repr += "], constant_objects=[";
    for (size_t i = 0; i < constant_objects.size(); i++) {
      repr += constant_objects[i];
      if (i < constant_objects.size() - 1) {
        repr += ", ";
      }
    }
    repr += "])";
    return repr;
  }
}  // namespace planning
