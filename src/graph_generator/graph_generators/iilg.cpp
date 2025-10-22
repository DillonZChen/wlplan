#include "../../../include/graph_generator/graph_generators/iilg.hpp"

namespace wlplan::graph_generator {
  IILGGenerator::IILGGenerator(const planning::Domain &domain) : ILGGenerator(domain, true) {}

  void IILGGenerator::set_problem(const planning::Problem &problem) {
    ILGGenerator::set_problem(problem);
    Graph graph = *base_graph;

    for (const auto &object : problem.get_constant_objects()) {
      obj_to_colour[object] = -obj_to_colour.size();
      graph.change_node_colour(object, obj_to_colour[object]);
      colour_to_description[obj_to_colour[object]] = "obj " + object;
    }
    for (const auto &object : problem.get_problem_objects()) {
      obj_to_colour[object] = -obj_to_colour.size();
      graph.change_node_colour(object, obj_to_colour[object]);
      colour_to_description[obj_to_colour[object]] = "obj " + object;
    }
  }
}  // namespace wlplan::graph_generator
