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
    PLOIGGenerator(const planning::Domain &domain, const bool differentiate_constant_objects);

    // Graph generation
    void set_problem(const planning::Problem &problem) override;
    std::shared_ptr<Graph> to_graph(const planning::State &state) override;
    // TODO implement optimised variant
    std::shared_ptr<Graph> to_graph_opt(const planning::State &state) override;
    void reset_graph() const override{};

    // Graph features
    int get_n_features() const override { return colour_to_description.size(); }
    int get_n_relations() const override { return n_relations; };

   protected:
    int n_relations;
    std::unordered_map<std::string, std::map<std::pair<int, int>, int>> ag_to_e_col;
    std::unordered_map<std::string, std::map<std::pair<int, int>, int>> ug_to_e_col;
    std::unordered_map<std::string, std::map<std::pair<int, int>, int>> ap_to_e_col;
  };
}  // namespace graph_generator

#endif  // GRAPH_GENERATOR_GRAPH_GENERATORS_PLOIG_GENERATOR_HPP
