#ifndef GRAPH_PLOIG_GENERATOR_HPP
#define GRAPH_PLOIG_GENERATOR_HPP

#include "../planning/atom.hpp"
#include "../planning/domain.hpp"
#include "../planning/problem.hpp"
#include "../planning/state.hpp"
#include "graph.hpp"
#include "graph_generator.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace graph {
  class PLOIGGenerator : public GraphGenerator {
   public:
    PLOIGGenerator(const planning::Domain &domain, bool differentiate_constant_objects);

    void set_problem(const planning::Problem &problem) override;
    std::shared_ptr<Graph> to_graph(const planning::State &state) override;
    // TODO currently this just reuses to_graph
    std::shared_ptr<Graph> to_graph_opt(const planning::State &state) override;
    void reset_graph() const override;

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
}  // namespace graph

#endif  // GRAPH_PLOIG_GENERATOR_HPP
