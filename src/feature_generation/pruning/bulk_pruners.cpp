#include "../../../include/feature_generation/dependency_graph.hpp"
#include "../../../include/feature_generation/features.hpp"

const int DISTINCT = -1;

namespace feature_generation {

  std::vector<int> Features::features_to_prune(std::vector<Embedding> X) {
    if (pruning == PruningOptions::COLLAPSE_ALL) {
      return greedy_all_pruner(X);
    } else {
      return std::vector<int>();
    }
  }

  inline void log_feature_info(std::vector<int> feature_group, std::vector<int> group_size) {
    int n_distinct_features = 0;
    int n_equivalent_features = 0;
    int n_equivalence_groups = 0;
    for (const int group : feature_group) {
      if (group == DISTINCT) {
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
    std::cout << "  Distinct features: " << n_distinct_features << std::endl;
    std::cout << "  Equivalent features: " << n_equivalent_features << std::endl;
    std::cout << "  Equivalence groups: " << n_equivalence_groups << std::endl;
    std::cout << "  Lower bound features: " << n_distinct_features + n_equivalence_groups
              << std::endl;
  }

  std::vector<int> Features::greedy_all_pruner(std::vector<Embedding> X) {
    std::cout << "Minimising equivalent features..." << std::endl;
    FeatureDependencyGraph fdg = FeatureDependencyGraph(colour_hash);

    // 1. compute equivalent features candidates
    std::cout << "Computing equivalent feature candidates." << std::endl;
    std::unordered_map<std::vector<int>, int, int_vector_hasher> canonical_group;
    int n_features = X.at(0).size();
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

    // mark distinct features
    for (int colour = 0; colour < n_features; colour++) {
      int group = feature_group.at(colour);
      if (group >= 0 && group_size.at(group) <= 1) {
        feature_group[colour] = DISTINCT;
      }
    }
    log_feature_info(feature_group, group_size);

    // 2. mark features that cannot be thrown out from highest iteration down
    std::cout << "Pruning features via dependency graph." << std::endl;
    for (int itr = iterations; itr >= 0; itr--) {
      for (const int colour : layer_to_colours[itr]) {
        if (feature_group.at(colour) != DISTINCT) {
          continue;
        }
        for (const int ancestor_colour : fdg.get_bw_edges(colour)) {
          int ancestor_group = feature_group.at(ancestor_colour);
          feature_group.at(ancestor_colour) = DISTINCT;
          if (ancestor_group == DISTINCT) {
            continue;
          }
          group_size.at(ancestor_group)--;
        }
      }
    }

    // remark distinct features
    for (int colour = 0; colour < n_features; colour++) {
      int group = feature_group.at(colour);
      if (group >= 0 && group_size.at(group) <= 1) {
        feature_group[colour] = DISTINCT;
      }
    }
    log_feature_info(feature_group, group_size);

    // 3. greedy pruning
    std::cout << "Greedy pruning." << std::endl;

    std::cout << "TODO implement" << std::endl;
    log_feature_info(feature_group, group_size);

    std::cout << "Equivalent features minimised!" << std::endl;

    return std::vector<int>();
  }
}  // namespace feature_generation
