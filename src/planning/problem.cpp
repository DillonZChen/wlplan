#include "../../include/planning/problem.hpp"

#include <iostream>

namespace planning {
  Problem::Problem(const Domain &domain,
                   const std::vector<std::string> &objects,
                   const std::vector<Atom> &positive_goals,
                   const std::vector<Atom> &negative_goals)
      : domain(std::make_shared<Domain>(domain)),
        objects(objects),
        positive_goals(positive_goals),
        negative_goals(negative_goals) {

    std::set<std::string> constant_objects(domain.constant_objects.begin(),
                                           domain.constant_objects.end());
    for (const auto &object : objects) {
      if (constant_objects.count(object)) {
        std::string err_msg = object + " exists in the problem and is a constant object.";
        throw std::runtime_error(err_msg);
      }
    }
  }
}  // namespace planning
