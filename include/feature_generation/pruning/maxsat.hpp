#ifndef FEATURE_GENERATION_PRUNING_MAXSAT_HPP
#define FEATURE_GENERATION_PRUNING_MAXSAT_HPP

#include <string>
#include <vector>

namespace feature_generation {
  struct MaxSatClause {
    std::vector<int> variables;
    std::vector<bool> negated;
    int weight;
    bool hard;

    MaxSatClause(const std::vector<int> &variables,
                 const std::vector<bool> &negated,
                 const int weight,
                 const bool hard);

    int size() const { return variables.size(); }
  };

  class MaxSatProblem {
    std::vector<int> variables;
    std::vector<MaxSatClause> clauses;

   public:
    MaxSatProblem(const std::vector<MaxSatClause> &clauses);

    int get_n_variables() const { return variables.size(); }
    int get_n_clauses() const { return clauses.size(); }

    // https://maxsat-evaluations.github.io/2022/rules.html#input
    std::string to_string();
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_PRUNING_MAXSAT_HPP
