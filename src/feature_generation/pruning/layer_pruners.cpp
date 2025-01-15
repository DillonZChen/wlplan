#include "../../../include/feature_generation/features.hpp"

namespace feature_generation {

  std::vector<int>
  Features::features_to_prune_this_iteration(int iteration,
                                             std::vector<std::vector<int>> &cur_colours) {
    if (pruning == PruningOptions::COLLAPSE_LAYER) {
      return greedy_iteration_pruner(iteration, cur_colours);
    } else {
      return std::vector<int>();
    }
  }

  std::vector<int> Features::greedy_iteration_pruner(int iteration,
                                                     std::vector<std::vector<int>> &cur_colours) {
    std::set<int> colours = get_iteration_colours(iteration);
    std::vector<int> features_to_prune;

    std::map<int, std::vector<int>> columns;
    size_t n_graphs = cur_colours.size();

    for (int colour : colours) {
      columns[colour] = std::vector<int>(n_graphs, 0);
    }
    for (size_t graph_i = 0; graph_i < n_graphs; graph_i++) {
      for (size_t node_i = 0; node_i < cur_colours[graph_i].size(); node_i++) {
        int colour = cur_colours[graph_i][node_i];
        if (colours.count(colour) == 0) {
          // this occurs for isolated nodes and would cause seg fault below
          continue;
        }
        columns[colour][graph_i]++;
      }
    }

    // greedily select first unique features
    std::unordered_set<std::vector<int>, int_vector_hasher> unique_features;
    for (int colour : colours) {
      std::vector<int> column = columns[colour];
      if (unique_features.count(column) == 0) {
        unique_features.insert(column);
      } else {
        // throw out because not unique
        features_to_prune.push_back(colour);
      }
    }

    return features_to_prune;
  }
}  // namespace feature_generation
