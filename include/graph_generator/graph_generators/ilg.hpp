#ifndef GRAPH_GENERATOR_GRAPH_GENERATORS_ILG
#define GRAPH_GENERATOR_GRAPH_GENERATORS_ILG

// Reference:
// Dillon Z. Chen, Felipe Trevizan, and Sylvie Thiébaux. Return to Tradition: Learning Reliable
// Heuristics with Classical Machine Learning. In ICAPS, 2024.

// Link:
// https://arxiv.org/abs/2403.16508

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

#define ILG_FACT_DESCRIPTIONS                                                                      \
  X(T_POS_GOAL, "T_POS_GOAL")                                                                      \
  X(F_POS_GOAL, "F_POS_GOAL")                                                                      \
  X(T_NEG_GOAL, "T_NEG_GOAL")                                                                      \
  X(F_NEG_GOAL, "F_NEG_GOAL")                                                                      \
  X(NON_GOAL, "NON_GOAL")                                                                          \
  X(_LAST, "_size_of_the_enum")

#define X(description, name) description,
enum class ILGFactDescription { ILG_FACT_DESCRIPTIONS };
#undef X

namespace graph_generator {
  class ILGGenerator : public GraphGenerator {
   public:
    ILGGenerator(const planning::Domain &domain, bool differentiate_constant_objects);

    void set_problem(const planning::Problem &problem) override;
    std::shared_ptr<Graph> to_graph(const planning::State &state) override;
    std::shared_ptr<Graph> to_graph_opt(const planning::State &state);
    void reset_graph() const;

   protected:
    /* The following variables remain constant for all problems */
    const planning::Domain &domain;
    const std::unordered_map<std::string, int> predicate_to_colour;
    bool differentiate_constant_objects;

    /* These variables get reset every time a new problem is set */
    std::shared_ptr<Graph> base_graph;
    std::unordered_set<std::string> positive_goal_names;
    std::unordered_set<std::string> negative_goal_names;
    std::shared_ptr<planning::Problem> problem;

    int fact_colour(const int predicate_idx, const ILGFactDescription &fact_description) const;
    int fact_colour(const planning::Atom &atom, const ILGFactDescription &fact_description) const;

    /* For modifying the base graph and redoing its changes */
    int n_nodes_added;
    std::vector<int> n_edges_added;
    std::vector<int> pos_goal_changed;
    std::vector<int> neg_goal_changed;
    std::vector<int> pos_goal_changed_pred;
    std::vector<int> neg_goal_changed_pred;
    std::shared_ptr<Graph> modify_graph_from_state(const planning::State &state,
                                                   const std::shared_ptr<Graph> graph,
                                                   bool store_changes);
  };

  inline int ILGGenerator::fact_colour(const int predicate_idx,
                                       const ILGFactDescription &fact_description) const {
    int pred_idx = predicate_idx * ((int)ILGFactDescription::_LAST);
    return 1 + pred_idx + (int)fact_description;
  }

  inline int ILGGenerator::fact_colour(const planning::Atom &atom,
                                       const ILGFactDescription &fact_description) const {
    return fact_colour(predicate_to_colour.at(atom.predicate->name), fact_description);
  }
}  // namespace graph_generator

#endif  // GRAPH_GENERATOR_GRAPH_GENERATORS_ILG
