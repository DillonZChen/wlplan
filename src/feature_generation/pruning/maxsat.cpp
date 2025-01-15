#include "../../../include/feature_generation/pruning/maxsat.hpp"

#include <iostream>

namespace feature_generation {
  MaxSatClause::MaxSatClause(const std::vector<int> &variables,
                             const std::vector<bool> &negated,
                             const int weight,
                             const bool hard)
      : variables(variables), negated(negated), weight(weight), hard(hard) {
    if (hard && weight != 0) {
      std::cout << "error: hard MaxSAT clauses should have 0 weight!" << std::endl;
      exit(-1);
    } else if (!hard && weight < 1) {
      std::cout << "error: soft MaxSAT clauses should have weight >= 1!" << std::endl;
      exit(-1);
    }
    if (variables.size() != negated.size()) {
      std::cout << "error: variables and negated in MaxSatClause should have the same size!" << std::endl;
      exit(-1);
    }
  }

  MaxSatProblem::MaxSatProblem(const std::vector<MaxSatClause> &clauses) : clauses(clauses) {
    // preprocess
  }

  std::string MaxSatProblem::to_string() {
    std::string ret = "";
    for (const MaxSatClause &clause : clauses) {
      std::string clause_string = "";
      if (clause.hard) {
        clause_string += "h ";
      } else {
        clause_string += std::to_string(clause.weight) + " ";
      }
      for (int i = 0; i < clause.size(); i++) {
        if (clause.negated[i]) {
          clause_string += "-";
        }
        clause_string += std::to_string(clause.variables[i]) + " ";
      }
      clause_string += "0";
      ret += clause_string + "\n";
    }
    return ret;
  }
}  // namespace feature_generation
