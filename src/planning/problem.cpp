#include "../../include/planning/problem.hpp"

#include "../../include/utils/strings.hpp"

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

  std::string Problem::to_string() const {
    std::string t1 = "\n  ";
    std::string t2 = "\n    ";

    std::string repr = "wlplan.planning.Problem(";

    repr += t1 + "domain=" + domain->to_string() + ",";

    repr += t1 + "objects=[";
    for (size_t i = 0; i < problem_objects.size(); i++) {
      repr += t2 + problem_objects[i] + ",";
    }
    repr += t1 + "],";

    repr += t1 + "statics=[";
    for (size_t i = 0; i < statics.size(); i++) {
      repr += t2 + statics[i].to_string() + ",";
    }
    repr += t1 + "],";

    repr += t1 + "fluents=[";
    for (size_t i = 0; i < fluents.size(); i++) {
      repr += t2 + fluents[i].to_string() + ": " + std::to_string(fluent_values[i]) + ",";
    }
    repr += t1 + "],";

    repr += t1 + "positive_goals=[";
    for (size_t i = 0; i < positive_goals.size(); i++) {
      repr += t2 + positive_goals[i].to_string() + ",";
    }
    repr += t1 + "],";

    repr += t1 + "negative_goals=[";
    for (size_t i = 0; i < negative_goals.size(); i++) {
      repr += t2 + negative_goals[i].to_string() + ",";
    }
    repr += t1 + "],";

    repr += t1 + "numeric_goals=[";
    for (size_t i = 0; i < numeric_goals.size(); i++) {
      repr += t2 + numeric_goals[i].to_string() + ",";
    }
    repr += t1 + "],";

    repr += "\n)";

    return repr;
  }

  bool Problem::operator==(const Problem &other) const {
    return *domain == *other.domain && problem_objects == other.problem_objects &&
           constant_objects == other.constant_objects && statics == other.statics &&
           fluents == other.fluents && fluent_values == other.fluent_values &&
           positive_goals == other.positive_goals && negative_goals == other.negative_goals &&
           numeric_goals == other.numeric_goals;
  }
}  // namespace planning
