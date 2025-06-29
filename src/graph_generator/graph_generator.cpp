#include "../../include/graph_generator/graph_generator.hpp"

namespace graph_generator {
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

  void GraphGenerator::print_init_colours() const {
    std::cout << "Initial node colours:" << std::endl;
    for (const auto &kv : colour_to_description) {
      std::cout << kv.first << " -> " << kv.second << std::endl;
    }
  }
}  // namespace graph_generator
