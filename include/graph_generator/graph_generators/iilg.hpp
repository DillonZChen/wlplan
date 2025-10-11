#ifndef GRAPH_GENERATOR_GRAPH_GENERATORS_IILG_HPP
#define GRAPH_GENERATOR_GRAPH_GENERATORS_IILG_HPP

#include "ilg.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wlplan::graph_generator {
  class IILGGenerator : public ILGGenerator {
   public:
    IILGGenerator(const planning::Domain &domain, const bool differentiate_constant_objects);

    // Graph generation
    void set_problem(const planning::Problem &problem) override;
    std::shared_ptr<Graph> to_graph(const planning::State &state) override;
    std::shared_ptr<Graph> to_graph_opt(const planning::State &state) override;

    // Graph features
    int get_n_features() const override {
      // same as ILG with an additional single, continuous value
      return colour_to_description.size() + 1;
    };
    int get_n_relations() const override {
      return std::max(domain.get_predicate_arity(), domain.get_function_arity());
    };

   protected:
    std::unordered_map<std::string, int> fluent_to_colour;
    int UNACHIEVED_GT_GOAL;
    int UNACHIEVED_GTEQ_GOAL;
    int UNACHIEVED_EQ_GOAL;
    int ACHIEVED_GT_GOAL;
    int ACHIEVED_GTEQ_GOAL;
    int ACHIEVED_EQ_GOAL;

    // Fluent values are given in every state.
    std::shared_ptr<Graph> modify_graph_from_numerics(const planning::State &state,
                                                      const std::shared_ptr<Graph> graph);
  };
}  // namespace wlplan::graph_generator

#endif  // GRAPH_GENERATOR_GRAPH_GENERATORS_IILG_HPP
