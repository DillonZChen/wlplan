#include "../../../include/feature_generation/feature_generators/ccwl.hpp"

#include "../../../include/graph/graph_generator_factory.hpp"
#include "../../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace feature_generation {
  CCWLFeatures::CCWLFeatures(const planning::Domain &domain,
                             std::string graph_representation,
                             int iterations,
                             std::string pruning,
                             bool multiset_hash)
      : WLFeatures("ccwl", domain, graph_representation, iterations, pruning, multiset_hash) {}

  CCWLFeatures::CCWLFeatures(const std::string &filename) : WLFeatures(filename) {}

  Embedding CCWLFeatures::embed_impl(const std::shared_ptr<graph::Graph> &graph) {
    // New additions to the WL algorithm are indicated with the [NUMERIC] comments.
    // We use a sum function for the pool operator as described in the ccWL algorithm.
    // To change this to max, we just need to replace += occurrences with std::max.

    /* 1. Initialise embedding before pruning, and set up memory */
    int categorical_size = get_n_features();
    Embedding x0(categorical_size * 2, 0);
    int n_nodes = graph->nodes.size();
    std::vector<int> colours(n_nodes);
    std::set<int> nodes = graph->get_nodes_set();

    /* 2. Compute initial colours */
    int col;
    int is_seen_colour;
    for (int node_i = 0; node_i < n_nodes; node_i++) {
      col = get_colour_hash({graph->nodes[node_i]}, 0);
      colours[node_i] = col;
      is_seen_colour = (col != UNSEEN_COLOUR);  // prevent branch prediction
      seen_colour_statistics[is_seen_colour][0]++;
      if (is_seen_colour) {
        x0[col]++;
        x0[col + categorical_size] += graph->node_values[node_i];  // [NUMERIC]
      }
    }

    /* 3. Main WL loop */
    for (int itr = 1; itr < iterations + 1; itr++) {
      refine(graph, nodes, colours, itr);
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

    return x0;
  }

  void CCWLFeatures::set_weights(const std::vector<double> &weights) {
    if (((int)weights.size()) != 2 * get_n_features()) {
      throw std::runtime_error("Number of weights must match twice the number of features.");
    }
    store_weights = true;
    this->weights = weights;
  }
}  // namespace feature_generation
