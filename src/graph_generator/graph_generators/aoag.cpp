#include "../../../include/graph_generator/graph_generators/aoag.hpp"

namespace graph_generator {
  AOAGGenerator::AOAGGenerator(const planning::Domain &domain,
                               const bool differentiate_constant_objects)
      : ILGGenerator(domain, differentiate_constant_objects, "AOAGGenerator") {
    // add schema colours
    int colour = 0;
    for (size_t i = 0; i < domain.schemata.size(); i++) {
      int colour = 0;
    }
  }

  std::shared_ptr<Graph> AOAGGenerator::to_graph(const planning::State &state) {
    throw NotSupportedError("AOAGGenerator.to_graph(state)");
  }

  std::shared_ptr<Graph> AOAGGenerator::to_graph(const planning::State &state,
                                                 const planning::Actions &actions) {
    std::shared_ptr<Graph> graph = ILGGenerator::to_graph(state);

    int action_node, object_node;
    std::string action_node_str, schema_name;

    for (const auto &action : actions) {
      // add node
      action_node_str = action->to_string();
      schema_name = action->schema->name;
      action_node = graph->add_node(action_node_str, schema_to_graph_colour[schema_name]);

      // add edges
      for (size_t r = 0; r < action->objects.size(); r++) {
        object_node = graph->get_node_index(action->objects[r]);
        graph->add_edge(action_node, r, object_node);
        graph->add_edge(object_node, r, action_node);
      }
    }

    return graph;
  }

  std::shared_ptr<Graph> AOAGGenerator::to_graph_opt(const planning::State &state) {
    throw NotSupportedError("AOAGGenerator.to_graph_opt(state)");
  }

  void AOAGGenerator::reset_graph() const {
    throw NotImplementedError("AOAGGenerator.reset_graph()");
  }
}  // namespace graph_generator
