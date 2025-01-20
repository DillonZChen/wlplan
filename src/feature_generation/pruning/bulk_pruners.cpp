#include "../../../include/feature_generation/features.hpp"
#include "../../../include/feature_generation/maxsat.hpp"

const int KEEP = -1;

namespace feature_generation {

  void Features::prune_bulk(const std::vector<graph::Graph> &graphs) {
    std::set<int> to_prune;
    if (pruning == PruningOptions::COLLAPSE_ALL) {
      collected = true;
      std::vector<Embedding> X = embed_graphs(graphs);
      to_prune = prune_maxsat(X);
    } else {
      to_prune = std::set<int>();
    }

    if (to_prune.size() != 0) {
      remap_colour_hash(to_prune);
    }
  }

  inline int mark_distinct_features(std::map<int, int> &feature_group,
                                    std::map<int, int> &group_size) {
    int changed = 0;
    std::set<int> distinct_groups;
    std::set<int> distinct_features;

    // mark groups with size <= 1 and their corresponding feature as distinct
    for (const auto &[colour, group] : feature_group) {
      if (group >= 0 && group_size.at(group) <= 1) {  // feature became distinct for keeping
        changed++;
        distinct_features.insert(colour);
        distinct_groups.insert(group);
      }
    }

    // kept features should be erased
    for (const int colour : distinct_features) {
      feature_group.erase(colour);
    }
    for (const int group : distinct_groups) {
      group_size.erase(group);
    }

    return changed;
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

    // 1. compute equivalent features candidates
    std::cout << "Computing equivalent feature candidates." << std::endl;
    std::map<int, int> feature_group = get_equivalence_groups(X);
    std::map<int, int> group_size;
    for (const auto &[_, group] : feature_group) {
      if (group_size.count(group) == 0) {
        group_size[group] = 0;
      }
      group_size[group]++;
    }

    mark_distinct_features(feature_group, group_size);

    // 2. mark features that should not be thrown out from highest iteration down
    std::cout << "Pruning features via dependency graph." << std::endl;
    int dp_iterations = 0;
    while (true) {
      std::cout << "DP iteration=" << dp_iterations << ". ";
      dp_iterations++;
      int changed = 0;
      for (int itr = iterations; itr >= 0; itr--) {
        for (const int colour : layer_to_colours[itr]) {
          if (feature_group.count(colour)) {
            continue;
          }
          for (const int ancestor_colour : edges_bw.at(colour)) {
            // mark prune candidates as distinct, so skip non-candidates
            if (!feature_group.count(ancestor_colour)) {
              continue;
            }
            int ancestor_group = feature_group.at(ancestor_colour);
            group_size.at(ancestor_group)--;
            changed++;
          }
        }
      }

      changed += mark_distinct_features(feature_group, group_size);
      std::cout << "changed: " << changed << ". candidates: " << feature_group.size() << std::endl;
      if (changed == 0) {
        break;
      }
    }

    // 3. maxsat
    std::cout << "Encoding MaxSAT." << std::endl;

    // get groups
    std::map<int, std::vector<int>> group_to_features;
    for (const auto &[colour, group] : feature_group) {
      if (group_to_features.count(group) == 0) {
        group_to_features[group] = std::vector<int>();
      }
      group_to_features[group].push_back(colour);
    }

    std::vector<MaxSatClause> clauses;

    // variable=T indicates feature to be thrown out
    // equivalently, ~variable=T indicates feature to be kept
    for (const auto &[colour, _] : feature_group) {
      clauses.push_back(MaxSatClause({colour}, {false}, 1, false));
    }

    // a thrown out variable forces children to be thrown out
    // i.e., variable => child_1 & ... & child_n which is equivalent to
    // (~variable | child_1) & ... & (~variable | child_n)
    for (const auto &[ancestor, _] : feature_group) {
      for (const int child : edges_fw.at(ancestor)) {
#ifdef DEBUGMODE
        if (!feature_group.count(child)) {
          std::cout << "ERROR: child of prune candidate is not a candidate" << std::endl;
          exit(-1);
        }
#endif
        clauses.push_back(MaxSatClause({ancestor, child}, {true, false}, 0, true));
      }
    }

    // keep one feature from each equivalence group
    for (const auto &[group, features] : group_to_features) {
      std::vector<bool> negated(features.size(), true);
      clauses.push_back(MaxSatClause(features, negated, 0, true));
    }

    // solve
    MaxSatProblem max_sat_problem = MaxSatProblem(clauses);

    // time the solver
    std::map<int, int> solution = max_sat_problem.solve();

    std::set<int> to_prune;

    for (const auto &[colour, value] : solution) {
      if (value == 1) {
        to_prune.insert(colour);
      }
    }

    std::cout << "Equivalent features minimised!" << std::endl;
    std::cout << "  Features kept: " << n_features - to_prune.size() << std::endl;
    std::cout << "  Features pruned: " << to_prune.size() << std::endl;

    return to_prune;
  }
}  // namespace feature_generation
