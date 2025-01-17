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

  inline void log_feature_info(int n_features,
                               std::set<int> prune_candidates,
                               std::map<int, int> feature_group,
                               std::map<int, int> group_size) {
    int n_candidate_features = prune_candidates.size();
    int n_distinct_features = n_features - n_candidate_features;
    int n_equivalence_groups = group_size.size();
    for (const auto &[group, size] : group_size) {
      if (size <= 1) {
        std::cout << "error: equivalence groups should have size > 1" << std::endl;
      }
    }
    std::cout << "Distinct features: " << n_distinct_features << std::endl;
    std::cout << "Prune candidates: " << n_candidate_features << std::endl;
    std::cout << "Equivalence groups: " << n_equivalence_groups << std::endl;
    std::cout << "Lower bound features: " << n_distinct_features + n_equivalence_groups
              << std::endl;
  }

  inline int mark_distinct_features(std::set<int> &prune_candidates,
                                    std::map<int, int> &feature_group,
                                    std::map<int, int> &group_size) {
    int changed = 0;
    std::set<int> distinct_groups;
    std::set<int> distinct_features;

    // mark groups with size <= 1 and their corresponding feature as distinct
    for (const int colour : prune_candidates) {
      int group = feature_group.at(colour);
      if (group >= 0 && group_size.at(group) <= 1) {  // feature became distinct for keeping
        changed++;
        distinct_features.insert(colour);
        distinct_groups.insert(group);
      }
    }

    // kept features should be erased from feature_group and group_size
    for (const int colour : distinct_features) {
      feature_group.erase(colour);
      prune_candidates.erase(colour);
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
    std::vector<std::vector<int>> edges_fw =
        std::vector<std::vector<int>>(n_features, std::vector<int>());
    std::vector<std::vector<int>> edges_bw =
        std::vector<std::vector<int>>(n_features, std::vector<int>());
    for (const auto &[neighbours, colour] : colour_hash) {  // std::vector<int>, int
      std::vector<int> indices = get_neighbour_colour_indices(neighbours);
      for (const int i : indices) {
        int ancestor = neighbours[i];
        edges_fw.at(ancestor).push_back(colour);
        edges_bw.at(colour).push_back(ancestor);
#ifdef DEBUGMODE
        std::cout << "FDG " << ancestor << " -> " << colour << std::endl;
#endif
      }
    }

    // 1. compute equivalent features candidates
    std::cout << "Computing equivalent feature candidates." << std::endl;
    std::unordered_map<std::vector<int>, int, int_vector_hasher> canonical_group;
    std::map<int, int> group_size;
    std::map<int, int> feature_group;
    std::set<int> prune_candidates;
    for (int colour = 0; colour < n_features; colour++) {
      prune_candidates.insert(colour);

      std::vector<int> feature;
      for (size_t j = 0; j < X.size(); j++) {
        feature.push_back(X[j][colour]);
      }

      int group;
      if (canonical_group.count(feature) == 0) {  // new feature
        group = canonical_group.size();
        canonical_group[feature] = group;
      } else {  // seen this feature before
        group = canonical_group.at(feature);
      }

      if (group_size.count(group) == 0) {
        group_size[group] = 0;
      }
      group_size.at(group)++;
      feature_group[colour] = group;
    }

    mark_distinct_features(prune_candidates, feature_group, group_size);
    // log_feature_info(n_features, prune_candidates, feature_group, group_size);

    // 2. mark features that should not be thrown out from highest iteration down
    std::cout << "Pruning features via dependency graph." << std::endl;
    int dp_iterations = 0;
    while (true) {
      std::cout << "DP iteration=" << dp_iterations << ". ";
      dp_iterations++;
      int changed = 0;
      for (int itr = iterations; itr >= 0; itr--) {
        for (const int colour : layer_to_colours[itr]) {
          if (prune_candidates.count(colour)) {
            continue;
          }
          for (const int ancestor_colour : edges_bw.at(colour)) {
            // mark prune candidates as distinct, so skip non-candidates
            if (!prune_candidates.count(ancestor_colour)) {
              continue;
            }
            int ancestor_group = feature_group.at(ancestor_colour);
            group_size.at(ancestor_group)--;
            prune_candidates.erase(ancestor_colour);
            changed++;
          }
        }
      }

      changed += mark_distinct_features(prune_candidates, feature_group, group_size);
      std::cout << "changed: " << changed << ". candidates: " << prune_candidates.size() << std::endl;
      if (changed == 0) {
        break;
      }
    }

    // log_feature_info(n_features, prune_candidates, feature_group, group_size);

    // 3. maxsat
    std::cout << "Encoding MaxSAT." << std::endl;

    // get groups
    std::map<int, std::vector<int>> group_to_features;
    for (const int colour : prune_candidates) {
      int group = feature_group.at(colour);
      if (group_to_features.count(group) == 0) {
        group_to_features[group] = std::vector<int>();
      }
      group_to_features[group].push_back(colour);
    }

    std::vector<MaxSatClause> clauses;

    // variable=T indicates feature to be thrown out
    // equivalently, ~variable=T indicates feature to be kept
    for (const int colour : prune_candidates) {
      clauses.push_back(MaxSatClause({colour}, {false}, 1, false));
    }

    // a thrown out variable forces children to be thrown out
    // i.e., variable => child_1 & ... & child_n which is equivalent to
    // (~variable | child_1) & ... & (~variable | child_n)
    for (const int ancestor : prune_candidates) {
      for (const int child : edges_fw.at(ancestor)) {
        if (!prune_candidates.count(child)) {
          std::cout << "error: child of prune candidate is not a candidate" << std::endl;
          exit(-1);
        }
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
    int n_to_keep = 0;

    for (const auto &[colour, value] : solution) {
      if (value == 1) {
        to_prune.insert(colour);
      } else {
        n_to_keep++;
      }
    }

    std::cout << "Equivalent features minimised!" << std::endl;
    std::cout << "  Features kept: " << n_to_keep << std::endl;
    std::cout << "  Features pruned: " << to_prune.size() << std::endl;

    return to_prune;
  }
}  // namespace feature_generation
