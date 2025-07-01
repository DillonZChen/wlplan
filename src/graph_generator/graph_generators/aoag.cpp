#include "../../../include/graph_generator/graph_generators/aoag.hpp"

namespace graph_generator {
  AOAGGenerator::AOAGGenerator(const planning::Domain &domain, bool differentiate_constant_objects)
      : ILGGenerator(domain, differentiate_constant_objects) {}

  std::shared_ptr<Graph> AOAGGenerator::to_graph(const planning::State &state) {
    throw std::runtime_error("AOAG.to_graph() must require action inputs.");
  }

  std::shared_ptr<Graph> AOAGGenerator::to_graph(const planning::State &state,
                                                 const std::vector<planning::Action> &actions) {
    std::shared_ptr<Graph> graph = ILGGenerator::to_graph(state);
    // TODO add action nodes
    throw NotImplementedException("AOAGGenerator.to_graph()");
    return graph;
  }

  std::shared_ptr<Graph> AOAGGenerator::to_graph_opt(const planning::State &state) {
    throw std::runtime_error("AOAG.to_graph() must require action inputs.");
  }

  void AOAGGenerator::reset_graph() const {
    throw NotImplementedException("AOAGGenerator.reset_graph()");
  }
}  // namespace graph_generator
