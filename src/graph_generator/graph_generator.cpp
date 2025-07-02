#include "../../include/graph_generator/graph_generator.hpp"

#include "../../include/utils/exceptions.hpp"

namespace graph_generator {
  GraphGenerator::GraphGenerator(const planning::Domain &domain,
                                 const bool differentiate_constant_objects,
                                 const std::string &graph_generator_name)
      : domain(domain),
        differentiate_constant_objects(differentiate_constant_objects),
        graph_generator_name(graph_generator_name) {
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

  std::shared_ptr<Graph> GraphGenerator::to_graph(const planning::State &state,
                                                  const planning::Actions &actions) {
    planning::ActionPointers action_pointers;
    for (const auto &action : actions) {
      action_pointers.push_back(std::make_shared<planning::Action>(action));
    }
    return to_graph(state, action_pointers);
  }

  std::vector<Graph> GraphGenerator::to_graphs(const data::DomainDataset dataset) {
    std::vector<Graph> graphs;

    const std::vector<data::ProblemDataset> &data = dataset.data;
    for (size_t i = 0; i < data.size(); i++) {
      const auto &d = data.at(i);
      const auto &problem = d.problem;
      const auto &states = d.states;
      const auto &actions = d.actions;
      set_problem(problem);
      for (size_t j = 0; j < states.size(); j++) {
        planning::ActionPointers action_pointers;
        for (const auto &action : actions.at(j)) {
          action_pointers.push_back(std::make_shared<planning::Action>(action));
        }
        graphs.push_back(*(to_graph(states.at(j), action_pointers)));
      }
    }

    return graphs;
  }

  int GraphGenerator::get_n_features() const { return colour_to_description.size(); }

  void GraphGenerator::print_init_colours() const {
    std::cout << "Initial node colours:" << std::endl;
    for (const auto &kv : colour_to_description) {
      std::cout << kv.first << " -> " << kv.second << std::endl;
    }
  }
}  // namespace graph_generator
