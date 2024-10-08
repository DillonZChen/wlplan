#include "../../include/feature_generation/ccwl_features.hpp"

#include "../../include/graph/graph_generator_factory.hpp"
#include "../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace feature_generation {
  CCWLFeatures::CCWLFeatures(const planning::Domain &domain,
                             std::string graph_representation,
                             int iterations,
                             std::string prune_features,
                             bool multiset_hash)
      : WLFeatures("ccwl",
                   domain,
                   graph_representation,
                   iterations,
                   prune_features,
                   multiset_hash) {}

  CCWLFeatures::CCWLFeatures(const std::string &filename) : WLFeatures(filename) {}

  Embedding CCWLFeatures::embed(const std::shared_ptr<graph::Graph> &graph) {
    collecting = false;
    if (!collected) {
      throw std::runtime_error("CCWLFeatures::collect() must be called before embedding");
    }

    // New additions to the WL algorithm are indicated with the [NUMERIC] comments.
    // We use a sum function for the pool operator as described in the ccWL algorithm.
    // To change this to max, we just need to replace += occurrences with std::max.

    /* 1. Initialise embedding before pruning */
    int categorical_size = colour_hash.size();
    Embedding x0(categorical_size * 2, 0);

    /* 2. Set up memory for WL updates */
    int n_nodes = graph->nodes.size();
    std::vector<int> colours(n_nodes);
    std::vector<int> colours_tmp(n_nodes);

    /* 3. Compute initial colours */
    int col;
    for (int node_i = 0; node_i < n_nodes; node_i++) {
      col = get_colour_hash({graph->nodes[node_i]});
      colours[node_i] = col;
      if (col != UNSEEN_COLOUR) {
        x0[col]++;
        x0[col + categorical_size] += graph->node_values[node_i];  // [NUMERIC]
      }
    }

    /* 4. Main WL loop */
    int is_seen_colour;
    for (int itr = 0; itr < iterations; itr++) {
      refine(graph, colours, colours_tmp);
      for (int node_i = 0; node_i < n_nodes; node_i++) {
        col = colours[node_i];
        is_seen_colour = (col != UNSEEN_COLOUR);  // prevent branch prediction
        seen_colour_statistics[is_seen_colour][itr]++;
        if (is_seen_colour) {
          x0[col]++;
          x0[col + categorical_size] += graph->node_values[node_i];  // [NUMERIC]
        }
      }
    }

    /* 5. Prune features with colours_to_keep */
    if (prune_features == "no_prune") {
      return x0;
    }

    int colours_to_keep_size = colours_to_keep.size();
    Embedding x(colours_to_keep_size * 2, 0);
    for (int i = 0; i < colours_to_keep_size; i++) {
      x[i] = x0[colours_to_keep[i]];
      x[i + colours_to_keep_size] = x0[colours_to_keep[i] + categorical_size];  // [NUMERIC]
    }
    return x;
  }

  void CCWLFeatures::set_weights(const std::vector<double> &weights) {
    if (((int)weights.size()) != 2 * get_n_features()) {
      throw std::runtime_error("Number of weights must match twice the number of features.");
    }
    store_weights = true;
    this->weights = weights;
  }
}  // namespace feature_generation
