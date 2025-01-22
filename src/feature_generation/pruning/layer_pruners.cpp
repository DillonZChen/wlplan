#include "../../../include/feature_generation/features.hpp"

namespace feature_generation {

  void Features::prune_this_iteration(int iteration,
                                      const std::vector<graph::Graph> &graphs,
                                      std::vector<std::vector<int>> &cur_colours) {
    std::set<int> to_prune;
    pruned = true;
    if (pruning == PruningOptions::COLLAPSE_LAYER) {
      to_prune = prune_collapse_layer(iteration, cur_colours);
    } else if (pruning == PruningOptions::COLLAPSE_LAYER_X) {
      to_prune = prune_collapse_layer_x(iteration, graphs, cur_colours);
    } else if (pruning == PruningOptions::COLLAPSE_LAYER_Y) {
      to_prune = prune_collapse_layer_y(iteration, graphs, cur_colours);
    } else {
      to_prune = std::set<int>();
      pruned = false;
    }

    if (to_prune.size() != 0) {
      std::cout << "Pruning " << to_prune.size() << " features." << std::endl;
      std::map<int, int> remap = remap_colour_hash(to_prune);
      for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
        for (size_t node_i = 0; node_i < cur_colours[graph_i].size(); node_i++) {
          int col = cur_colours[graph_i][node_i];
          if (remap.count(col) > 0) {
            cur_colours[graph_i][node_i] = remap[col];
          } else {
            cur_colours[graph_i][node_i] = UNSEEN_COLOUR;
          }
        }
      }
    }
  }

  std::set<int> Features::prune_collapse_layer(int iteration,
                                               std::vector<std::vector<int>> &cur_colours) {
    std::set<int> colours = get_iteration_colours(iteration);
    std::set<int> features_to_prune;

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
        features_to_prune.insert(colour);
      }
    }

    return features_to_prune;
  }

  std::set<int> Features::prune_collapse_layer_x(int iteration,
                                                 const std::vector<graph::Graph> &graphs,
                                                 std::vector<std::vector<int>> &cur_colours) {
    int original_iterations = iterations;
    iterations = iteration;
    collecting = false;
    collected = true;

    std::set<int> features_to_prune;
    std::vector<Embedding> X = embed_graphs(graphs);
    std::map<int, int> feature_group = get_equivalence_groups(X);
    std::map<int, std::vector<int>> group_to_features;
    for (const auto &[colour, group] : feature_group) {
      if (group_to_features.count(group) == 0) {
        group_to_features[group] = std::vector<int>();
      }
      group_to_features[group].push_back(colour);
    }
    for (const auto &[_, features] : group_to_features) {
      if (features.size() == 1) {
        continue;
      }
      bool canonicalised = false;
      for (int feature : features) {
        if (!canonicalised) {
          canonicalised = true;
          continue;
        } else if (colour_to_layer[feature] == iteration) {
          features_to_prune.insert(feature);
        }
      }
    }

    collecting = true;
    collected = false;
    iterations = original_iterations;

    return features_to_prune;
  }

  std::set<int> Features::prune_collapse_layer_y(int iteration,
                                                 const std::vector<graph::Graph> &graphs,
                                                 std::vector<std::vector<int>> &cur_colours) {
    int original_iterations = iterations;
    iterations = iteration;
    collecting = false;
    collected = true;

    std::vector<Embedding> X = embed_graphs(graphs);
    std::set<int> to_prune = prune_maxsat_x(X, iterations);

    collecting = true;
    collected = false;
    iterations = original_iterations;

    return to_prune;
  }
}  // namespace feature_generation
