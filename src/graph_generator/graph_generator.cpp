#include "../../include/graph_generator/graph_generator.hpp"

namespace graph_generator {
  GraphGenerator::GraphGenerator(const planning::Domain &domain,
                                 bool differentiate_constant_objects)
      : domain(domain), differentiate_constant_objects(differentiate_constant_objects) {
    /* We assume all graphs have object nodes */

    // add constant object colours
    if (differentiate_constant_objects) {
      for (size_t i = 0; i < domain.constant_objects.size(); i++) {
        int colour = -(i + 1);
        colour_to_description[colour] = domain.constant_objects[i] + " _CONSTANT_";
      }
    }

    // add normal object colour
    colour_to_description[0] = "_OBJECT_";
  }

  std::vector<graph_generator::Graph> GraphGenerator::to_graphs(const data::DomainDataset dataset) {
    std::vector<graph_generator::Graph> graphs;

    const std::vector<data::ProblemDataset> &data = dataset.data;
    for (size_t i = 0; i < data.size(); i++) {
      const auto &problem_states = data[i];
      const auto &problem = problem_states.problem;
      const auto &states = problem_states.states;
      const auto &actions = problem_states.actions;
      set_problem(problem);
      for (size_t j = 0; j < states.size(); j++) {
        graphs.push_back(*(to_graph(states[j], actions[j])));
      }
    }

    return graphs;
  }

  std::shared_ptr<Graph> GraphGenerator::to_graph(const planning::State &state,
                                                  const std::vector<planning::Action> &actions) {
    return to_graph(state);
  }

  int GraphGenerator::get_n_features() const { return colour_to_description.size(); }

  void GraphGenerator::print_init_colours() const {
    std::cout << "Initial node colours:" << std::endl;
    for (const auto &kv : colour_to_description) {
      std::cout << kv.first << " -> " << kv.second << std::endl;
    }
  }
}  // namespace graph_generator
