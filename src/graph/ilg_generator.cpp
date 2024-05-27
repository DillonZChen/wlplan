#include "../../include/graph/ilg_generator.hpp"

#define X(description, name) name,
char const *fact_description_name[] = {ILG_FACT_DESCRIPTIONS};
#undef X

namespace graph {
  ILGGenerator::ILGGenerator(const planning::Domain &domain)
      : domain(domain), predicate_to_colour(domain.predicate_to_colour) {
    /// initialise initial node colours
    // add constant object colours
    for (size_t i = 0; i < domain.constant_objects.size(); i++) {
      int colour = -(i + 1);
      colour_to_description[colour] = "_constant_object_ " + domain.constant_objects[i];
    }

    colour_to_description[0] = "_object_";

    // add predicate colours
    for (size_t i = 0; i < domain.predicates.size(); i++) {
      for (int j = 0; j < (int)ILGFactDescription::_LAST; j++) {
        int colour = fact_colour(i, (ILGFactDescription)j);
        std::string desc = domain.predicates[i].name + " " + fact_description_name[j];
        colour_to_description[colour] = desc;
      }
    }
  }

  void ILGGenerator::set_problem(const planning::Problem &problem) {
    // reset graph and variables
    Graph graph = Graph(/*store_node_names=*/true);
    positive_goal_names = std::unordered_set<std::string>();
    negative_goal_names = std::unordered_set<std::string>();

    /// add nodes
    // add constant object nodes
    for (size_t i = 0; i < domain.constant_objects.size(); i++) {
      std::string node = domain.constant_objects[i];
      int colour = -(i + 1);
      graph.add_node(node, colour);
    }

    // objects
    for (const auto &object : problem.objects) {
      std::string node = object;
      int colour = 0;
      graph.add_node(node, colour);
    }

    // atoms
    for (const auto &atom : problem.positive_goals) {
      std::string node = atom.to_string();
      int colour = fact_colour(atom, ILGFactDescription::F_POS_GOAL);
      graph.add_node(node, colour);
      positive_goal_names.insert(node);
    }

    for (const auto &atom : problem.negative_goals) {
      std::string node = atom.to_string();
      int colour = fact_colour(atom, ILGFactDescription::F_NEG_GOAL);
      graph.add_node(node, colour);
      negative_goal_names.insert(node);
    }

    /// add labelled edges
    // add edges atoms <-> objects
    for (const auto &atom : problem.positive_goals) {
      for (size_t r = 0; r < atom.objects.size(); r++) {
        std::string atom_node = atom.to_string();
        std::string object_node = atom.objects[r];
        graph.add_edge(atom_node, r, object_node);
        graph.add_edge(object_node, r, atom_node);
      }
    }

    /// set pointer
    base_graph = std::make_shared<Graph>(graph);
  }

  std::shared_ptr<Graph> ILGGenerator::to_graph(const planning::State &state) const {
    std::shared_ptr<Graph> graph = std::make_shared<Graph>(*base_graph);

    for (const auto &atom : state) {
      std::string atom_node = atom.to_string();
      if (positive_goal_names.count(atom_node)) {
        graph->change_node_colour(atom_node, fact_colour(atom, ILGFactDescription::T_POS_GOAL));
      } else if (negative_goal_names.count(atom_node)) {
        graph->change_node_colour(atom_node, fact_colour(atom, ILGFactDescription::T_NEG_GOAL));
      } else {
        int colour = fact_colour(atom, ILGFactDescription::NON_GOAL);
        graph->add_node(atom_node, colour);
        for (size_t r = 0; r < atom.objects.size(); r++) {
          std::string object_node = atom.objects[r];
          graph->add_edge(atom_node, r, object_node);
          graph->add_edge(object_node, r, atom_node);
        }
      }
    }

    return graph;
  }

  int ILGGenerator::get_n_edge_labels() const { return domain.max_arity(); }

  std::vector<int> ILGGenerator::get_init_colours() const {
    std::vector<int> init_colours;
    for (const auto &kv : colour_to_description) {
      init_colours.push_back(kv.first);
    }
    return init_colours;
  }

  void ILGGenerator::print_init_colours() const {
    std::cout << "Initial ILG node colours:" << std::endl;
    for (const auto &kv : colour_to_description) {
      std::cout << kv.first << " -> " << kv.second << std::endl;
    }
  }
}  // namespace graph
