#include "../../include/graph/nilg_generator.hpp"

namespace graph {
  NILGGenerator::NILGGenerator(const planning::Domain &domain, bool differentiate_constant_objects)
      : ILGGenerator(domain, differentiate_constant_objects) {

    // add function colours
    for (size_t i = 0; i < domain.functions.size(); i++) {
      int colour = colour_to_description.rbegin()->first + 1;  // keys sorted in ascending order
      std::string function_name = domain.functions[i].name;
      colour_to_description[colour] = function_name;
      fluent_to_colour[function_name] = colour;
    }

    // add numeric goal colours
    UNACHIEVED_GT_GOAL = colour_to_description.rbegin()->first + 1;
    colour_to_description[UNACHIEVED_GT_GOAL] = "_UNACHIEVED >  GOAL_";
    UNACHIEVED_GTEQ_GOAL = colour_to_description.rbegin()->first + 1;
    colour_to_description[UNACHIEVED_GTEQ_GOAL] = "_UNACHIEVED >= GOAL_";
    UNACHIEVED_EQ_GOAL = colour_to_description.rbegin()->first + 1;
    colour_to_description[UNACHIEVED_EQ_GOAL] = "_UNACHIEVED == GOAL_";
    ACHIEVED_GT_GOAL = colour_to_description.rbegin()->first + 1;
    colour_to_description[ACHIEVED_GT_GOAL] = "_ACHIEVED >  GOAL_";
    ACHIEVED_GTEQ_GOAL = colour_to_description.rbegin()->first + 1;
    colour_to_description[ACHIEVED_GTEQ_GOAL] = "_ACHIEVED >= GOAL_";
    ACHIEVED_EQ_GOAL = colour_to_description.rbegin()->first + 1;
    colour_to_description[ACHIEVED_EQ_GOAL] = "_ACHIEVED == GOAL_";
  }

  void NILGGenerator::set_problem(const planning::Problem &problem) {
    ILGGenerator::set_problem(problem);
    Graph graph = *base_graph;

    // add fluents
    std::vector<planning::Fluent> fluents = problem.get_fluents();
    std::vector<double> fluent_values = problem.get_fluent_values();
    for (size_t i = 0; i < fluents.size(); i++) {
      // add nodes
      planning::Fluent fluent = fluents[i];
      std::string fluent_node = fluent.to_string();
      int colour = fluent_to_colour[fluent.function->name];
      graph.add_node(fluent_node, colour, fluent_values[i]);

      // add edges
      for (size_t r = 0; r < fluent.objects.size(); r++) {
        std::string object_node = fluent.objects[r];
        graph.add_edge(fluent_node, r, object_node);
        graph.add_edge(object_node, r, fluent_node);
      }
    }

    // add numeric goals
    std::vector<planning::NumericCondition> numeric_goals = problem.get_numeric_goals();
    for (size_t i = 0; i < numeric_goals.size(); i++) {
      // add nodes
      std::string goal_node = "_numeric_goal_" + std::to_string(i);
      planning::NumericCondition goal = numeric_goals[i];
      // Goal colours and values should be computed for every state so it does
      // not matter what we initialise it to.
      int colour = 0;
      double value = 0;
      graph.add_node(goal_node, colour, value);

      // add edges
      for (int fluent_id : goal.get_fluent_ids()) {
        std::string fluent_node = fluents[fluent_id].to_string();
        graph.add_edge(goal_node, -1, fluent_node);
        graph.add_edge(fluent_node, -1, goal_node);
      }
    }

    // set pointer
    base_graph = std::make_shared<Graph>(graph);
    n_edges_added = std::vector<int>(base_graph->nodes.size(), 0);
  }

  std::shared_ptr<Graph>
  NILGGenerator::modify_graph_from_numerics(const planning::State &state,
                                            const std::shared_ptr<Graph> graph) {
    std::vector<planning::Fluent> fluents = problem->get_fluents();
    std::vector<double> fluent_values = state.values;
    for (size_t i = 0; i < fluents.size(); i++) {
      std::string fluent_node = fluents[i].to_string();
      int fluent_node_i = graph->get_node_index(fluent_node);
      graph->change_node_value(fluent_node_i, fluent_values[i]);
    }

    std::pair<bool, double> goal_eval;
    std::vector<planning::NumericCondition> num_goals = problem->get_numeric_goals();
    int goal_colour, goal_node_i;
    for (size_t i = 0; i < num_goals.size(); i++) {
      std::string goal_node = "_numeric_goal_" + std::to_string(i);
      goal_node_i = graph->get_node_index(goal_node);
      goal_eval = num_goals[i].evaluate_formula_and_error(state.values);
      graph->change_node_value(goal_node_i, goal_eval.second);

      switch (num_goals[i].get_comparator_type()) {
      case planning::ComparatorType::GreaterThan:
        goal_colour = goal_eval.first ? ACHIEVED_GT_GOAL : UNACHIEVED_GT_GOAL;
        break;
      case planning::ComparatorType::GreaterThanOrEqual:
        goal_colour = goal_eval.first ? ACHIEVED_GTEQ_GOAL : UNACHIEVED_GTEQ_GOAL;
        break;
      default:  // case planning::ComparatorType::Equal:
        goal_colour = goal_eval.first ? ACHIEVED_EQ_GOAL : UNACHIEVED_EQ_GOAL;
        break;
      }
      graph->change_node_colour(goal_node_i, goal_colour);
    }

    return graph;
  }

  std::shared_ptr<Graph> NILGGenerator::to_graph(const planning::State &state) {
    std::shared_ptr<Graph> graph = ILGGenerator::to_graph(state);
    graph = modify_graph_from_numerics(state, graph);
    return graph;
  }

  std::shared_ptr<Graph> NILGGenerator::to_graph_opt(const planning::State &state) {
    base_graph = ILGGenerator::to_graph_opt(state);
    base_graph = modify_graph_from_numerics(state, base_graph);
    return base_graph;
  }
}  // namespace graph
