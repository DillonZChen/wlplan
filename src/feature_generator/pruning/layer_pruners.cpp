#include "../../../include/feature_generator/features.hpp"

namespace wlplan {
  namespace feature_generator {

    void Features::prune_this_iteration(int iteration,
                                        const std::vector<graph_generator::Graph> &graphs,
                                        std::vector<std::vector<int>> &cur_colours) {
      std::set<int> to_prune;
      pruned = true;
      if (pruning == PruningOptions::LAYER_GREEDY) {
        to_prune = prune_collapse_layer_greedy(iteration, graphs);
      } else if (pruning == PruningOptions::LAYER_MAXSAT) {
        to_prune = prune_collapse_layer_maxsat(iteration, graphs);
      } else if (pruning == PruningOptions::LAYER_FREQUENCY) {
        to_prune = prune_collapse_layer_frequency(iteration, graphs);
      } else if (pruning == PruningOptions::LAYER_MAXSAT_FREQUENCY) {
        to_prune = prune_collapse_layer_maxsat(iteration, graphs);
        std::set<int> to_prune_f = prune_collapse_layer_frequency(iteration, graphs);
        to_prune.insert(to_prune_f.begin(), to_prune_f.end());
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

    std::set<int>
    Features::prune_collapse_layer_greedy(int iteration,
                                          const std::vector<graph_generator::Graph> &graphs) {
      // As in Bonet et al. 2019
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

    std::set<int>
    Features::prune_collapse_layer_maxsat(int iteration,
                                          const std::vector<graph_generator::Graph> &graphs) {
      // MaxSAT but per layer
      int original_iterations = iterations;
      iterations = iteration;
      collecting = false;
      collected = true;

      std::vector<Embedding> X = embed_graphs(graphs);
      std::set<int> to_prune = prune_maxsat(X, iterations);

      collecting = true;
      collected = false;
      iterations = original_iterations;

      return to_prune;
    }

    std::set<int>
    Features::prune_collapse_layer_frequency(int iteration,
                                             const std::vector<graph_generator::Graph> &graphs) {
      // Frequency count < 1% of n_data
      int original_iterations = iterations;
      iterations = iteration;
      collecting = false;
      collected = true;

      std::set<int> to_prune;
      std::vector<Embedding> X = embed_graphs(graphs);
      int N = (int)X.size();
      int D = (int)X.at(0).size();
      int one_percent = N / 100;
      for (int i = 0; i < D; i++) {
        int count = 0;
        for (int j = 0; j < N; j++) {
          count += X.at(j).at(i);
        }
        if (count <= one_percent) {
          to_prune.insert(i);
        }
      }

      collecting = true;
      collected = false;
      iterations = original_iterations;

      std::cout << "Pruning " << to_prune.size() << " features with <1% frequency count."
                << std::endl;

      return to_prune;
    }
  }  // namespace feature_generator
}  // namespace wlplan
