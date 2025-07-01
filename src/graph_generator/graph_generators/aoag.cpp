#include "../../../include/graph_generator/graph_generators/aoag.hpp"

namespace graph_generator {
  AOAGGenerator::AOAGGenerator(const planning::Domain &domain,
                               const bool differentiate_constant_objects)
      : ILGGenerator(domain, differentiate_constant_objects, "AOAGGenerator") {}

  std::shared_ptr<Graph> AOAGGenerator::to_graph(const planning::State &state) {
    throw NotSupportedError("AOAGGenerator.to_graph(state)");
  }

  std::shared_ptr<Graph> AOAGGenerator::to_graph(const planning::State &state,
                                                 const std::vector<planning::Action> &actions) {
    std::shared_ptr<Graph> graph = ILGGenerator::to_graph(state);
    // TODO add action nodes
    throw NotImplementedError("AOAGGenerator.to_graph(state, actions)");
    return graph;
  }

  std::shared_ptr<Graph> AOAGGenerator::to_graph_opt(const planning::State &state) {
    throw NotSupportedError("AOAGGenerator.to_graph_opt(state)");
  }

  void AOAGGenerator::reset_graph() const {
    throw NotImplementedError("AOAGGenerator.reset_graph()");
  }
}  // namespace graph_generator
