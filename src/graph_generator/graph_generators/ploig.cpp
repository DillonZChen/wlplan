#include "../../../include/graph_generator/graph_generators/ploig.hpp"

namespace wlplan {
  namespace graph_generator {
    PLOIGGenerator::PLOIGGenerator(const planning::Domain &domain,
                                   const bool differentiate_constant_objects)
        : GraphGenerator(domain, differentiate_constant_objects, "PLOIGGenerator") {
      if (differentiate_constant_objects) {
        throw std::runtime_error("PLOIG does not support differentiating constant objects");
      }

      n_relations = 0;
      std::string desc;
      for (const auto &predicate : domain.predicates) {
        if (predicate.arity == 1) {
          unary_pred_to_i[predicate.name] = unary_pred_to_i.size();
        }

        std::map<std::pair<int, int>, int> ag;
        std::map<std::pair<int, int>, int> ug;
        std::map<std::pair<int, int>, int> ap;
        for (int i = 0; i < predicate.arity; i++) {
          for (int j = i + 1; j < predicate.arity; j++) {
            desc = predicate.name + "__AG " + std::to_string(i) + " " + std::to_string(j);
            colour_to_description[n_relations] = desc;
            ag[std::make_pair(i, j)] = n_relations;
            n_relations++;

            desc = predicate.name + "__AG " + std::to_string(j) + " " + std::to_string(i);
            colour_to_description[n_relations] = desc;
            ag[std::make_pair(j, i)] = n_relations;
            n_relations++;

            desc = predicate.name + "__UG " + std::to_string(i) + " " + std::to_string(j);
            colour_to_description[n_relations] = desc;
            ug[std::make_pair(i, j)] = n_relations;
            n_relations++;

            desc = predicate.name + "__UG " + std::to_string(j) + " " + std::to_string(i);
            colour_to_description[n_relations] = desc;
            ug[std::make_pair(j, i)] = n_relations;
            n_relations++;

            desc = predicate.name + "__AP " + std::to_string(i) + " " + std::to_string(j);
            colour_to_description[n_relations] = desc;
            ap[std::make_pair(i, j)] = n_relations;
            n_relations++;

            desc = predicate.name + "__AP " + std::to_string(j) + " " + std::to_string(i);
            colour_to_description[n_relations] = desc;
            ap[std::make_pair(j, i)] = n_relations;
            n_relations++;
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
        throw std::runtime_error("PLOIG does not yet support negative goals");
      }
      obj_to_colour.clear();
      for (const auto &object : problem.get_constant_objects()) {
        obj_to_colour[object] = 0;
      }
      for (const auto &object : problem.get_problem_objects()) {
        obj_to_colour[object] = 0;
      }

      for (const auto &atom : problem.get_positive_goals()) {
        positive_goal_names.insert(atom.to_string());
        if (atom.predicate->arity == 1) {
          int pred_i = unary_pred_to_i.at(atom.predicate->name);
          obj_to_colour[atom.objects[0]] = 2 << (pred_i + unary_pred_to_i.size());
        }
      }

      Graph graph = Graph(/*store_node_names=*/true);

      /* add nodes */
      for (const auto &pair : obj_to_colour) {
        graph.add_node(pair.first, pair.second);
      }

      /* set pointer */
      base_graph = std::make_shared<Graph>(graph);
    }

    std::shared_ptr<Graph>
    PLOIGGenerator::modify_graph_from_state(const planning::State &state,
                                            const std::shared_ptr<Graph> graph) {
      std::unordered_map<std::string, int> obj_to_colour_copy = obj_to_colour;

      /* add edges */

      // compute ug, ag, ap
      std::unordered_set<std::string> unachieved_goals;
      std::vector<std::shared_ptr<planning::Atom>> ug;
      std::vector<std::shared_ptr<planning::Atom>> ag;
      std::vector<std::shared_ptr<planning::Atom>> ap;
      for (const auto &atom : problem->get_positive_goals()) {
        unachieved_goals.insert(atom.to_string());
      }
      for (const auto &atom : state.atoms) {
        if (atom->predicate->arity == 1) {
          obj_to_colour_copy[atom->objects[0]] = 2 << (unary_pred_to_i[atom->predicate->name]);
        }

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

      // update node colours
      for (const auto &pair : obj_to_colour_copy) {
        base_graph->change_node_colour(pair.first, pair.second);
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
            graph->add_edge(object_node_i, mapper[std::make_pair(i, j)], object_node_j);
            graph->add_edge(object_node_j, mapper[std::make_pair(j, i)], object_node_i);
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
            graph->add_edge(object_node_i, mapper[std::make_pair(i, j)], object_node_j);
            graph->add_edge(object_node_j, mapper[std::make_pair(j, i)], object_node_i);
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
            graph->add_edge(object_node_i, mapper[std::make_pair(i, j)], object_node_j);
            graph->add_edge(object_node_j, mapper[std::make_pair(j, i)], object_node_i);
          }
        }
      }

      return graph;
    }

    void PLOIGGenerator::reset_graph() const {
      // Delete all edges, keep nodes
      for (size_t i = 0; i < base_graph->nodes.size(); i++) {
        base_graph->edges[i].clear();
      }
      for (const auto &pair : obj_to_colour) {
        base_graph->change_node_colour(pair.first, pair.second);
      }
    }

    std::shared_ptr<Graph> PLOIGGenerator::to_graph(const planning::State &state) {
      std::shared_ptr<Graph> graph = std::make_shared<Graph>(*base_graph);
      graph = modify_graph_from_state(state, graph);
      return graph;
    }

    std::shared_ptr<Graph> PLOIGGenerator::to_graph(const planning::State &state,
                                                    const planning::ActionPointers &actions) {
      // action-agnostic
      (void)actions;
      return to_graph(state);
    }

    std::shared_ptr<Graph> PLOIGGenerator::to_graph_opt(const planning::State &state) {
      base_graph = modify_graph_from_state(state, base_graph);
      return base_graph;
    }

  }  // namespace graph_generator
}  // namespace wlplan
