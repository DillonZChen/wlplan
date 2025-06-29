#ifndef GRAPH_GENERATOR_GRAPH_GENERATORS_PLOIG_GENERATOR_HPP
#define GRAPH_GENERATOR_GRAPH_GENERATORS_PLOIG_GENERATOR_HPP

// Reference:
// Tom Silver, Rohan Chitnis, Aidan Curtis, Joshua B. Tenenbaum, Tomás Lozano-Pérez, Leslie Pack
// Kaelbling. Planning with Learned Object Importance in Large Problem Instances using Graph Neural
// Networks. In AAAI, 2021.

// Link:
// https://arxiv.org/abs/2009.05613

#include "../../planning/atom.hpp"
#include "../../planning/domain.hpp"
#include "../../planning/problem.hpp"
#include "../../planning/state.hpp"
#include "../graph.hpp"
#include "../graph_generator.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace graph_generator {
  class PLOIGGenerator : public GraphGenerator {
   public:
    PLOIGGenerator(const planning::Domain &domain, bool differentiate_constant_objects);

    void set_problem(const planning::Problem &problem) override;
    std::shared_ptr<Graph> to_graph(const planning::State &state) override;
    // TODO implement optimised variant
    std::shared_ptr<Graph> to_graph_opt(const planning::State &state) override {
      return to_graph(state);
    };
    void reset_graph() const override{};

   protected:
    /* The following variables remain constant for all problems */
    const planning::Domain &domain;
    std::unordered_map<std::string, std::map<std::pair<int, int>, int>> ag_to_e_col;
    std::unordered_map<std::string, std::map<std::pair<int, int>, int>> ug_to_e_col;
    std::unordered_map<std::string, std::map<std::pair<int, int>, int>> ap_to_e_col;
    bool differentiate_constant_objects;

    /* These variables get reset every time a new problem is set */
    std::shared_ptr<planning::Problem> problem;
    std::unordered_set<std::string> positive_goal_names;
  };
}  // namespace graph_generator

#endif  // GRAPH_GENERATOR_GRAPH_GENERATORS_PLOIG_GENERATOR_HPP
