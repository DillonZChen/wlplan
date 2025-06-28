#include "../../../include/graph/graph_generators/ploig.hpp"

namespace graph {
  PLOIGGenerator::PLOIGGenerator(const planning::Domain &domain,
                                 bool differentiate_constant_objects)
      : domain(domain), differentiate_constant_objects(differentiate_constant_objects) {
    // initialise initial node colours
    if (differentiate_constant_objects) {
      // add constant object colours
      for (size_t i = 0; i < domain.constant_objects.size(); i++) {
        int colour = -(i + 1);
        colour_to_description[colour] = domain.constant_objects[i] + " _CONSTANT_";
      }
    }

    colour_to_description[0] = "_OBJECT_";

    // add predicate colours
    int col = 1;
    std::string desc;
    for (const auto &predicate : domain.predicates) {
      std::map<std::pair<int, int>, int> ag;
      std::map<std::pair<int, int>, int> ug;
      std::map<std::pair<int, int>, int> ap;
      for (int i = 0; i < predicate.arity; i++) {
        for (int j = i + 1; j < predicate.arity; j++) {
          desc = predicate.name + "__AG " + std::to_string(i) + " " + std::to_string(j);
          colour_to_description[col] = desc;
          ag[std::make_pair(i, j)] = col;
          col++;

          desc = predicate.name + "__AG " + std::to_string(j) + " " + std::to_string(i);
          colour_to_description[col] = desc;
          ag[std::make_pair(j, i)] = col;
          col++;

          desc = predicate.name + "__UG " + std::to_string(i) + " " + std::to_string(j);
          colour_to_description[col] = desc;
          ug[std::make_pair(i, j)] = col;
          col++;

          desc = predicate.name + "__UG " + std::to_string(j) + " " + std::to_string(i);
          colour_to_description[col] = desc;
          ug[std::make_pair(j, i)] = col;
          col++;

          desc = predicate.name + "__AP " + std::to_string(i) + " " + std::to_string(j);
          colour_to_description[col] = desc;
          ap[std::make_pair(i, j)] = col;
          col++;

          desc = predicate.name + "__AP " + std::to_string(j) + " " + std::to_string(i);
          colour_to_description[col] = desc;
          ap[std::make_pair(j, i)] = col;
          col++;
        }
      }
      ag_to_e_col[predicate.name] = ag;
      ug_to_e_col[predicate.name] = ug;
      ap_to_e_col[predicate.name] = ap;
    }
  }

  void PLOIGGenerator::set_problem(const planning::Problem &problem) {
    this->problem = std::make_shared<planning::Problem>(problem);
    if (problem.get_negative_goals().size() > 0) {
      std::cerr << "PLOIGGenerator does not support negative goals. "
                << "Please use ILGGenerator instead." << std::endl;
      exit(1);
      throw std::runtime_error("PLOIG does not yet support negative");
    }
    for (const auto &atom : problem.get_positive_goals()) {
      positive_goal_names.insert(atom.to_string());
    }
  }

  std::shared_ptr<Graph> PLOIGGenerator::to_graph(const planning::State &state) {
    // reset graph and variables
    Graph graph = Graph(/*store_node_names=*/true);

    /* add nodes */
    int colour;

    // add constant object nodes
    for (size_t i = 0; i < problem->get_constant_objects().size(); i++) {
      std::string node = domain.constant_objects[i];
      if (differentiate_constant_objects) {
        colour = -(i + 1);
      } else {
        colour = 0;
      }
      graph.add_node(node, colour);
    }

    // objects
    for (const auto &object : problem->get_problem_objects()) {
      std::string node = object;
      colour = 0;
      graph.add_node(node, colour);
    }

    /* add edges */

    // compute ug, ag, ap
    std::unordered_set<std::string> unachieved_goals;
    for (const auto &atom : problem->get_positive_goals()) {
      unachieved_goals.insert(atom.to_string());
    }
    std::vector<std::shared_ptr<planning::Atom>> ug;
    std::vector<std::shared_ptr<planning::Atom>> ag;
    std::vector<std::shared_ptr<planning::Atom>> ap;
    for (const auto &atom : state.atoms) {
      std::string atom_name = atom->to_string();
      if (positive_goal_names.count(atom_name)) {
        unachieved_goals.erase(atom_name);
        ag.push_back(atom);
      } else {
        ap.push_back(atom);
      }
    }
    for (const auto &atom : problem->get_positive_goals()) {
      std::string atom_name = atom.to_string();
      if (unachieved_goals.count(atom_name)) {
        ug.push_back(std::make_shared<planning::Atom>(atom));
      }
    }

    // add obj <-> obj edges
    for (const auto &atom : ag) {
      std::string predicate = atom->predicate->name;
      std::vector<std::string> objects = atom->objects;
      int arity = (int)objects.size();
      std::map<std::pair<int, int>, int> mapper = ag_to_e_col[predicate];
      for (int i = 0; i < arity; i++) {
        for (int j = i + 1; j < arity; j++) {
          std::string object_node_i = objects[i];
          std::string object_node_j = objects[j];
          graph.add_edge(object_node_i, mapper[std::make_pair(i, j)], object_node_j);
          graph.add_edge(object_node_j, mapper[std::make_pair(j, i)], object_node_i);
        }
      }
    }

    for (const auto &atom : ug) {
      std::string predicate = atom->predicate->name;
      std::vector<std::string> objects = atom->objects;
      int arity = (int)objects.size();
      std::map<std::pair<int, int>, int> mapper = ug_to_e_col[predicate];
      for (int i = 0; i < arity; i++) {
        for (int j = i + 1; j < arity; j++) {
          std::string object_node_i = objects[i];
          std::string object_node_j = objects[j];
          graph.add_edge(object_node_i, mapper[std::make_pair(i, j)], object_node_j);
          graph.add_edge(object_node_j, mapper[std::make_pair(j, i)], object_node_i);
        }
      }
    }

    for (const auto &atom : ap) {
      std::string predicate = atom->predicate->name;
      std::vector<std::string> objects = atom->objects;
      int arity = (int)objects.size();
      std::map<std::pair<int, int>, int> mapper = ap_to_e_col[predicate];
      for (int i = 0; i < arity; i++) {
        for (int j = i + 1; j < arity; j++) {
          std::string object_node_i = objects[i];
          std::string object_node_j = objects[j];
          graph.add_edge(object_node_i, mapper[std::make_pair(i, j)], object_node_j);
          graph.add_edge(object_node_j, mapper[std::make_pair(j, i)], object_node_i);
        }
      }
    }

    return std::make_shared<Graph>(graph);
  }
}  // namespace graph
