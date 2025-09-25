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

namespace wlplan {
  namespace graph_generator {
    class ILGGenerator : public GraphGenerator {
     public:
      ILGGenerator(const planning::Domain &domain, const bool differentiate_constant_objects);
      ILGGenerator(const planning::Domain &domain,
                   const bool differentiate_constant_objects,
                   const std::string &derived_graph_generator_name);

      // Graph generation
      void set_problem(const planning::Problem &problem) override;
      std::shared_ptr<Graph> to_graph(const planning::State &state) override;
      std::shared_ptr<Graph> to_graph(const planning::State &state,
                                      const planning::ActionPointers &actions) override;
      std::shared_ptr<Graph> to_graph_opt(const planning::State &state) override;
      void reset_graph() const override;

      // Graph features
      int get_n_features() const override { return colour_to_description.size(); };
      int get_n_relations() const override { return domain.get_predicate_arity(); };

     protected:
      int fact_colour(const int predicate_idx, const ILGFactDescription &fact_description) const;
      int fact_colour(const planning::Atom &atom, const ILGFactDescription &fact_description) const;

      /* For modifying the base graph and redoing its changes */
      int n_nodes_added;
      std::vector<int> n_edges_added;
      std::vector<int> pos_goal_changed;
      std::vector<int> neg_goal_changed;
      std::vector<int> pos_goal_changed_pred;
      std::vector<int> neg_goal_changed_pred;
      std::unordered_map<std::string, int> object_node_indices;
      std::unordered_map<std::string, int> positive_goal_atom_indices;
      std::unordered_map<std::string, int> negative_goal_atom_indices;
      std::shared_ptr<Graph> modify_graph_from_state(const planning::State &state,
                                                     const std::shared_ptr<Graph> graph,
                                                     const bool store_changes);
    };

    inline int ILGGenerator::fact_colour(const int predicate_idx,
                                         const ILGFactDescription &fact_description) const {
      int pred_idx = predicate_idx * ((int)ILGFactDescription::_LAST);
      return 1 + pred_idx + (int)fact_description;
    }

    inline int ILGGenerator::fact_colour(const planning::Atom &atom,
                                         const ILGFactDescription &fact_description) const {
      return fact_colour(domain.predicate_to_colour.at(atom.predicate->name), fact_description);
    }
  }  // namespace graph_generator
}  // namespace wlplan

#endif  // GRAPH_GENERATOR_GRAPH_GENERATORS_ILG
