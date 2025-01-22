#include "../../../include/feature_generation/features.hpp"
#include "../../../include/feature_generation/maxsat.hpp"

#include <queue>

const int KEEP = -1;

namespace feature_generation {

  void Features::prune_bulk(const std::vector<graph::Graph> &graphs) {
    std::set<int> to_prune;
    pruned = true;
    if (pruning == PruningOptions::COLLAPSE_ALL) {
      collected = true;
      std::vector<Embedding> X = embed_graphs(graphs);
      to_prune = prune_maxsat(X);
    } else if (pruning == PruningOptions::COLLAPSE_ALL_X) {
      collected = true;
      std::vector<Embedding> X = embed_graphs(graphs);
      to_prune = prune_maxsat_x(X);
    } else {
      to_prune = std::set<int>();
      pruned = false;
    }

    if (to_prune.size() != 0) {
      remap_colour_hash(to_prune);
    }
  }

  std::set<int> Features::prune_maxsat(std::vector<Embedding> X) {
    std::cout << "Minimising equivalent features..." << std::endl;

    // 0. construct feature dependency graph
    int n_features = X.at(0).size();
    std::vector<std::set<int>> edges_fw = std::vector<std::set<int>>(n_features, std::set<int>());
    std::vector<std::set<int>> edges_bw = std::vector<std::set<int>>(n_features, std::set<int>());

    for (int itr = 1; itr < iterations + 1; itr++) {
      // neighbours: std::vector<int>; colour: int
      for (const auto &[neighbours, colour] : colour_hash[itr]) {
        for (const int ancestor : neighbour_container->get_neighbour_colours(neighbours)) {
          edges_fw.at(ancestor).insert(colour);
          edges_bw.at(colour).insert(ancestor);
        }
      }
    }

#ifdef DEBUGMODE
    for (int colour = 0; colour < n_features; colour++) {
      for (const int child : edges_fw.at(colour)) {
        std::cout << "FDG " << colour << " -> " << child << std::endl;
      }
    }
#endif

    // 1. compute equivalence groups
    std::cout << "Computing equivalence groups." << std::endl;
    std::map<int, int> feature_group = get_equivalence_groups(X);
    std::map<int, std::set<int>> group_to_features;
    for (const auto &[feature, group] : feature_group) {
      if (group_to_features.count(group) == 0) {
        group_to_features[group] = std::set<int>();
      }
      group_to_features[group].insert(feature);
    }

    std::cout << "  Current prune candidates: " << feature_group.size() << std::endl;

    // 2. mark features that should not be thrown out from highest iteration down
    std::cout << "Marking distinct features via dependency graph." << std::endl;
    std::queue<int> q;
    for (int colour = 0; colour < n_features; colour++) {
      // mark all distinct features by putting in to queue
      if (group_to_features.at(feature_group.at(colour)).size() == 1) {
        q.push(colour);
      }
    }

    while (!q.empty()) {
      int colour = q.front();
      q.pop();

      // already marked as distinct so skip
      if (feature_group.count(colour) == 0) {
        continue;
      }

      // mark as distinct
      int group = feature_group.at(colour);
      group_to_features[group].erase(colour);
      feature_group.erase(colour);

      // add ancestors to queue
      for (const int ancestor_colour : edges_bw.at(colour)) {
        if (feature_group.count(ancestor_colour)) {
          q.push(ancestor_colour);
        }
      }

      // check group distinct and add to queue if so
      if (group_to_features[group].size() <= 1) {
        for (const int feature : group_to_features[group]) {
          q.push(feature);
        }
        group_to_features.erase(group);
      }
    }

    std::cout << "  Current prune candidates: " << feature_group.size() << std::endl;

    // 3. maxsat
    std::cout << "Encoding MaxSAT." << std::endl;

    std::vector<MaxSatClause> clauses;

    // variable=T indicates feature to be thrown out
    // equivalently, ~variable=T indicates feature to be kept
    for (const auto &[colour, _] : feature_group) {
      clauses.push_back(MaxSatClause({colour + 1}, {false}, 1, false));
    }

    // a thrown out variable forces children to be thrown out
    // i.e., ancestor => child_1 & ... & child_n which is equivalent to
    // (~ancestor | child_1) & ... & (~ancestor | child_n)
    for (const auto &[ancestor, _] : feature_group) {
      for (const int child : edges_fw.at(ancestor)) {
#ifdef DEBUGMODE
        if (!feature_group.count(child)) {
          std::cout << "ERROR: child of prune candidate is not a candidate" << std::endl;
          exit(-1);
        }
#endif
        clauses.push_back(MaxSatClause({ancestor + 1, child + 1}, {true, false}, 0, true));
      }
    }

    // keep one feature from each equivalence group
    for (const auto &[group, features] : group_to_features) {
      std::vector<int> features_v;
      for (const int feature : features) {
        features_v.push_back(feature + 1);
      }
      std::vector<bool> negated(features.size(), true);
      clauses.push_back(MaxSatClause(features_v, negated, 0, true));
    }

    // solve
    MaxSatProblem max_sat_problem = MaxSatProblem(clauses);

    // time the solver
    std::map<int, int> solution = max_sat_problem.solve();

    std::set<int> to_prune;

    for (const auto &[colour, value] : solution) {
      if (value == 1) {
        to_prune.insert(colour - 1);
      }
    }

    std::cout << "Equivalent features minimised!" << std::endl;
    std::cout << "  Features kept: " << n_features - to_prune.size() << std::endl;
    std::cout << "  Features pruned: " << to_prune.size() << std::endl;

    return to_prune;
  }

  std::set<int> Features::prune_maxsat_x(std::vector<Embedding> X) {
    // Same as prune_maxsat but no marking distinct features via dependency graph, and just
    // letting maxsat deal with this
    std::cout << "Minimising equivalent features..." << std::endl;

    // 0. construct feature dependency graph
    int n_features = X.at(0).size();
    std::vector<std::set<int>> edges_fw = std::vector<std::set<int>>(n_features, std::set<int>());
    std::vector<std::set<int>> edges_bw = std::vector<std::set<int>>(n_features, std::set<int>());

    for (int itr = 1; itr < iterations + 1; itr++) {
      // neighbours: std::vector<int>; colour: int
      for (const auto &[neighbours, colour] : colour_hash[itr]) {
        for (const int ancestor : neighbour_container->get_neighbour_colours(neighbours)) {
          edges_fw.at(ancestor).insert(colour);
          edges_bw.at(colour).insert(ancestor);
        }
      }
    }

#ifdef DEBUGMODE
    for (int colour = 0; colour < n_features; colour++) {
      for (const int child : edges_fw.at(colour)) {
        std::cout << "FDG " << colour << " -> " << child << std::endl;
      }
    }
#endif

    // 1. compute equivalence groups
    std::cout << "Computing equivalence groups." << std::endl;
    std::map<int, int> feature_group = get_equivalence_groups(X);
    std::map<int, std::set<int>> group_to_features;
    for (const auto &[feature, group] : feature_group) {
      if (group_to_features.count(group) == 0) {
        group_to_features[group] = std::set<int>();
      }
      group_to_features[group].insert(feature);
    }

    // 2. maxsat
    std::cout << "Encoding MaxSAT." << std::endl;

    std::vector<MaxSatClause> clauses;

    // variable=T indicates feature to be thrown out
    // equivalently, ~variable=T indicates feature to be kept
    for (const auto &[colour, _] : feature_group) {
      clauses.push_back(MaxSatClause({colour + 1}, {false}, 1, false));
    }

    // a kept variable forces ancestors to be kept
    // i.e. ~child => ~ancestor_1 & ... & ~ancestor_n which is equivalent to
    // (child | ~ancestor_1) & ... & (child | ~ancestor_n)
    for (const auto &[child, _] : feature_group) {
      for (const int ancestor : edges_bw.at(child)) {
        clauses.push_back(MaxSatClause({child + 1, ancestor + 1}, {false, true}, 0, true));
      }
    }

    // keep one feature from each equivalence group
    for (const auto &[group, features] : group_to_features) {
      std::vector<int> features_v;
      for (const int feature : features) {
        features_v.push_back(feature + 1);
      }
      std::vector<bool> negated(features.size(), true);
      clauses.push_back(MaxSatClause(features_v, negated, 0, true));
    }

    // solve
    MaxSatProblem max_sat_problem = MaxSatProblem(clauses);

    // time the solver
    std::map<int, int> solution = max_sat_problem.solve();

    std::set<int> to_prune;

    for (const auto &[colour, value] : solution) {
      if (value == 1) {
        to_prune.insert(colour - 1);
      }
    }

    std::cout << "Equivalent features minimised!" << std::endl;
    std::cout << "  Features kept: " << n_features - to_prune.size() << std::endl;
    std::cout << "  Features pruned: " << to_prune.size() << std::endl;

    return to_prune;
  }
}  // namespace feature_generation
