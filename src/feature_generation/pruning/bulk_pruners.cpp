#include "../../../include/feature_generation/features.hpp"
#include "../../../include/feature_generation/maxsat.hpp"

const int KEEP = -1;

namespace feature_generation {

  std::set<int> Features::features_to_prune(const std::vector<graph::Graph> &graphs) {
    if (pruning == PruningOptions::COLLAPSE_ALL) {
      collected = true;
      std::vector<Embedding> X = embed_graphs(graphs);
      return maxsat_bulk_pruner(X);
    } else {
      return std::set<int>();
    }
  }

  inline void log_feature_info(std::vector<int> feature_group, std::vector<int> group_size) {
    int n_distinct_features = 0;
    int n_equivalent_features = 0;
    int n_equivalence_groups = 0;
    for (const int group : feature_group) {
      if (group == KEEP) {
        n_distinct_features++;
      } else if (group_size.at(group) > 1) {
        n_equivalent_features++;
      } else {  // (group_size.at(group) <= 1)
        std::cout << "error: equivalence group has a distinct feature" << std::endl;
        exit(-1);
      }
    }
    for (const int size : group_size) {
      if (size > 1) {
        n_equivalence_groups++;
      }
    }
    std::cout << "Distinct features: " << n_distinct_features << std::endl;
    std::cout << "Equivalent features: " << n_equivalent_features << std::endl;
    std::cout << "Equivalence groups: " << n_equivalence_groups << std::endl;
    std::cout << "Lower bound features: " << n_distinct_features + n_equivalence_groups
              << std::endl;
  }

  inline bool mark_distinct_features(std::vector<int> &feature_group,
                                     std::vector<int> &group_size) {
    bool changed = false;
    for (size_t colour = 0; colour < feature_group.size(); colour++) {
      int group = feature_group.at(colour);
      if (group >= 0 && group_size.at(group) <= 1) {
        feature_group[colour] = KEEP;
        changed = true;
      }
    }
    return changed;
  }

  std::set<int> Features::maxsat_bulk_pruner(std::vector<Embedding> X) {
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
        // std::cout << "FDG " << ancestor << " ~> " << colour << std::endl;  // DEBUG
        edges_fw.at(ancestor).push_back(colour);
        edges_bw.at(colour).push_back(ancestor);
      }
    }

    // 1. compute equivalent features candidates
    std::cout << "Computing equivalent feature candidates." << std::endl;
    std::unordered_map<std::vector<int>, int, int_vector_hasher> canonical_group;
    std::vector<int> group_size(n_features, 0);
    std::vector<int> feature_group(n_features, 0);
    for (int colour = 0; colour < n_features; colour++) {
      std::vector<int> feature;
      for (size_t j = 0; j < X.size(); j++) {
        feature.push_back(X[j][colour]);
      }
      int group;
      if (canonical_group.count(feature) == 0) {
        group = canonical_group.size();
        canonical_group[feature] = group;
      } else {  // seen this feature before
        group = canonical_group.at(feature);
      }
      group_size.at(group)++;
      feature_group.at(colour) = group;
    }

    mark_distinct_features(feature_group, group_size);
    log_feature_info(feature_group, group_size);

    // 2. mark features that should not be thrown out from highest iteration down
    std::cout << "Pruning features via dependency graph." << std::endl;
    int dp_iterations = 0;
    while (true) {
      std::cout << "Pruning DP iteration=" << dp_iterations << std::endl;
      dp_iterations++;
      for (int itr = iterations; itr >= 0; itr--) {
        for (const int colour : layer_to_colours[itr]) {
          if (feature_group.at(colour) != KEEP) {
            continue;
          }
          for (const int ancestor_colour : edges_bw.at(colour)) {
            int ancestor_group = feature_group.at(ancestor_colour);
            feature_group.at(ancestor_colour) = KEEP;
            if (ancestor_group == KEEP) {
              continue;
            }
            group_size.at(ancestor_group)--;
          }
        }
      }

      bool changed = mark_distinct_features(feature_group, group_size);
      if (!changed) {
        break;
      }
    }

    log_feature_info(feature_group, group_size);

    // 3. maxsat
    std::cout << "Encoding MaxSAT." << std::endl;

    // get groups
    std::map<int, std::vector<int>> group_to_features;
    std::set<int> variables;
    for (int colour = 0; colour < n_features; colour++) {
      int group = feature_group.at(colour);
      if (group != KEEP) {
        if (group_to_features.count(group) == 0) {
          group_to_features[group] = std::vector<int>();
        }
        group_to_features[group].push_back(colour);
        variables.insert(colour);
      }
    }

    std::vector<MaxSatClause> clauses;

    // variable=T indicates feature to be thrown out
    // equivalently, ~variable=T indicates feature to be kept
    for (const int variable : variables) {
      clauses.push_back(MaxSatClause({variable}, {false}, 1, false));
    }

    // a thrown out variable forces children to be thrown out
    // i.e., variable => child_1 & ... & child_n which is equivalent to
    // (~variable | child_1) & ... & (~variable | child_n)
    for (const int ancestor : variables) {
      for (const int child : edges_fw.at(ancestor)) {
        if (variables.count(child) == 0) {
          continue;
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
