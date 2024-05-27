#ifndef PLANNING_PROBLEM_HPP
#define PLANNING_PROBLEM_HPP

#include "atom.hpp"
#include "domain.hpp"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace planning {
  using Assignment = std::vector<int>;

  class Problem {
   private:
    std::shared_ptr<Domain> domain;

   public:
    const std::vector<std::string> objects;
    const std::vector<Atom> positive_goals;
    const std::vector<Atom> negative_goals;

    Problem(const Domain &domain,
            const std::vector<std::string> &objects,
            const std::vector<Atom> &positive_goals,
            const std::vector<Atom> &negative_goals);

    Domain get_domain() const { return *domain; }
  };

}  // namespace planning

#endif  // PLANNING_PROBLEM_HPP
