#include "../../include/planning/problem.hpp"

#include "../../include/utils/tokenise.hpp"

#include <algorithm>
#include <iostream>

#define PYBIND11_DETAILED_ERROR_MESSAGES 1

namespace planning {
  Problem::Problem(const Domain &domain,
                   const std::vector<Object> &objects,
                   const std::vector<Atom> &statics,
                   const std::vector<Fluent> &fluents,
                   const std::vector<double> &fluent_values,
                   const std::vector<Atom> &positive_goals,
                   const std::vector<Atom> &negative_goals,
                   const std::vector<NumericCondition> &numeric_goals)
      : domain(std::make_shared<Domain>(domain)),
        statics(statics),
        fluents(fluents),
        fluent_values(fluent_values),
        positive_goals(positive_goals),
        negative_goals(negative_goals),
        numeric_goals(numeric_goals) {

    // handle objects
    int cnt = 0;
    for (const auto &object : domain.constant_objects) {
      object_to_id[object] = cnt;
      constant_objects_set.insert(object);
      constant_objects.push_back(object);
      cnt++;
    }

    for (const auto &object : objects) {
      if (constant_objects_set.count(object)) {
        continue;
      }
      object_to_id[object] = cnt;
      constant_objects_set.insert(object);
      problem_objects.push_back(object);
      cnt++;
    }

    // handle fluents
    if (fluents.size() != fluent_values.size()) {
      std::cout << "Error: Number of fluent variables and fluent values do not match." << std::endl;
      std::exit(1);
    }

    fluent_name_to_id.clear();
    for (size_t i = 0; i < fluents.size(); i++) {
      fluent_name_to_id[fluents[i].to_string()] = i;
    }
  }

  Problem::Problem(const Domain &domain,
                   const std::vector<Object> &objects,
                   const std::vector<Fluent> &fluents,
                   const std::vector<double> &fluent_values,
                   const std::vector<Atom> &positive_goals,
                   const std::vector<Atom> &negative_goals,
                   const std::vector<NumericCondition> &numeric_goals)
      : Problem(domain,
                objects,
                {},
                fluents,
                fluent_values,
                positive_goals,
                negative_goals,
                numeric_goals) {}

  Problem::Problem(const Domain &domain,
                   const std::vector<Object> &objects,
                   const std::vector<Atom> &positive_goals,
                   const std::vector<Atom> &negative_goals)
      : Problem(domain, objects, {}, {}, {}, positive_goals, negative_goals, {}) {}

  py::tuple Problem::__getstate__(const planning::Problem &input) {
    return py::make_tuple(Domain::__getstate__(input.get_domain()),
                          input.get_problem_objects(),
                          input.get_statics(),
                          input.get_fluents(),
                          input.get_fluent_values(),
                          input.get_positive_goals(),
                          input.get_negative_goals(),
                          input.get_numeric_goals());
  }

  Problem Problem::__setstate__(py::tuple t) {
    if (t.size() != 8) {
      throw std::runtime_error("Invalid state for Problem: expected 8 elements, got " +
                               std::to_string(t.size()));
    }
    return Problem(Domain::__setstate__(t[0].cast<py::tuple>()),
                   t[1].cast<std::vector<Object>>(),
                   t[2].cast<std::vector<Atom>>(),
                   t[3].cast<std::vector<Fluent>>(),
                   t[4].cast<std::vector<double>>(),
                   t[5].cast<std::vector<Atom>>(),
                   t[6].cast<std::vector<Atom>>(),
                   t[7].cast<std::vector<NumericCondition>>());
  }

  void Problem::dump() const {
    std::cout << "domain=" << domain->to_string() << std::endl;
    std::cout << "objects=[" << std::endl;
    for (const auto &object : problem_objects) {
      std::cout << "  " << object << std::endl;
    }
    std::cout << "]" << std::endl;
    std::cout << "statics=[" << std::endl;
    for (const auto &static_atom : statics) {
      std::cout << "  " << static_atom.to_string() << std::endl;
    }
    std::cout << "]" << std::endl;
    std::cout << "fluents=[" << std::endl;
    for (size_t i = 0; i < fluents.size(); i++) {
      std::cout << "  " << fluents[i].to_string() << " = " << fluent_values[i] << std::endl;
    }
    std::cout << "]" << std::endl;
    std::cout << "positive_goals=[" << std::endl;
    for (const auto &positive_goal : positive_goals) {
      std::cout << "  " << positive_goal.to_string() << std::endl;
    }
    std::cout << "]" << std::endl;
    std::cout << "negative_goals=[" << std::endl;
    for (const auto &negative_goal : negative_goals) {
      std::cout << "  " << negative_goal.to_string() << std::endl;
    }
    std::cout << "]" << std::endl;
    std::cout << "numeric_goals=[" << std::endl;
    for (const auto &numeric_goal : numeric_goals) {
      std::cout << "  " << numeric_goal.to_string() << std::endl;
    }
    std::cout << "]" << std::endl;
  }
}  // namespace planning
