#include "../../include/feature_generation/iwl_features.hpp"

#include "../../include/graph/graph_generator_factory.hpp"
#include "../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace feature_generation {
  IWLFeatures::IWLFeatures(const std::string feature_name,
                           const planning::Domain &domain,
                           std::string graph_representation,
                           int iterations,
                           std::string pruning,
                           bool multiset_hash)
      : Features(feature_name,
                 domain,
                 graph_representation,
                 iterations,
                 pruning,
                 multiset_hash) {}

  IWLFeatures::IWLFeatures(const planning::Domain &domain,
                           std::string graph_representation,
                           int iterations,
                           std::string pruning,
                           bool multiset_hash)
      : IWLFeatures("iwl",
                    domain,
                    graph_representation,
                    iterations,
                    pruning,
                    multiset_hash) {}

  IWLFeatures::IWLFeatures(const std::string &filename) : Features(filename) {}

  void IWLFeatures::refine(const std::shared_ptr<graph::Graph> &graph,
                           std::vector<int> &colours,
                           std::vector<int> &colours_tmp) {
    // memory for storing string and hashed int representation of colours
    std::vector<int> new_colour;
    std::vector<int> neighbour_vector;
    int new_colour_compressed;

    for (size_t u = 0; u < graph->nodes.size(); u++) {
      // skip unseen colours
      if (colours[u] == UNSEEN_COLOUR) {
        new_colour_compressed = UNSEEN_COLOUR;
        goto end_of_iteration;
      }
      neighbour_container->clear();

      for (const auto &edge : graph->edges[u]) {
        // skip unseen colours
        if (colours[edge.second] == UNSEEN_COLOUR) {
          new_colour_compressed = UNSEEN_COLOUR;
          goto end_of_iteration;
        }

        // add sorted neighbour (colour, edge_label) pair
        neighbour_container->insert(colours[edge.second], edge.first);
      }

      // add current colour and sorted neighbours into sorted colour key
      new_colour = {colours[u]};

      // TODO this can be optimised by not copying data and creating a hash on neighbour_container
      neighbour_vector = neighbour_container->to_vector();

      new_colour.insert(new_colour.end(), neighbour_vector.begin(), neighbour_vector.end());

      // hash seen colours
      new_colour_compressed = get_colour_hash(new_colour);

    end_of_iteration:
      colours_tmp[u] = new_colour_compressed;
    }

    colours.swap(colours_tmp);
  }

  void IWLFeatures::collect_main(const std::vector<graph::Graph> &graphs) {
    // intermediate graph colours during WL and extra memory for WL updates
    std::vector<int> colours;
    std::vector<int> colours_tmp;

    n_seen_graphs += graphs.size();
    for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
      const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
      int n_nodes = graph->nodes.size();
      int n_edges = graph->get_n_edges();
      n_seen_nodes += n_nodes;
      n_seen_edges += n_edges;

      // individualisation for each node
      for (int node_i = 0; node_i < n_nodes; node_i++) {
        colours = std::vector<int>(n_nodes, 0);
        colours_tmp = std::vector<int>(n_nodes, 0);

        // individualise node
        const int original_colour = graph->nodes[node_i];
        graph->change_node_colour(node_i, INDIVIDUALISE_COLOUR);

        // init colours
        cur_collecting_layer = 0;
        for (int u = 0; u < n_nodes; u++) {
          int col = get_colour_hash({graph->nodes[u]});
          colours[u] = col;
          seen_initial_colours.insert(col);
        }

        // main WL loop
        for (int iteration = 1; iteration < iterations + 1; iteration++) {
          cur_collecting_layer = iteration;
          refine(graph, colours, colours_tmp);
        }

        // reset node colour
        graph->change_node_colour(node_i, original_colour);
      }
    }
  }

  Embedding IWLFeatures::embed(const std::shared_ptr<graph::Graph> &graph) {
    collecting = false;
    if (!collected) {
      throw std::runtime_error("IWLFeatures::collect() must be called before embedding");
    }

    /* 1. Initialise embedding before pruning */
    Embedding x0(colour_hash.size(), 0);

    /* 2. Set up memory for WL updates */
    int n_nodes = graph->nodes.size();

    /* Individualisation */
    for (int node_i = 0; node_i < n_nodes; node_i++) {
      std::vector<int> colours(n_nodes);
      std::vector<int> colours_tmp(n_nodes);

      // individualise node
      const int original_colour = graph->nodes[node_i];
      graph->change_node_colour(node_i, INDIVIDUALISE_COLOUR);

      /* 3. Compute initial colours */
      for (int u = 0; u < n_nodes; u++) {
        int col = get_colour_hash({graph->nodes[u]});
        colours[u] = col;
        if (col != UNSEEN_COLOUR) {
          x0[col]++;
        }
      }

      /* 4. Main WL loop */
      int is_seen_colour;
      for (int itr = 0; itr < iterations; itr++) {
        refine(graph, colours, colours_tmp);
        for (const int col : colours) {
          is_seen_colour = (col != UNSEEN_COLOUR);  // prevent branch prediction
          seen_colour_statistics[is_seen_colour][itr]++;
          if (is_seen_colour) {
            x0[col]++;
          }
        }
      }

      // reset node colour
      graph->change_node_colour(node_i, original_colour);
    }
    
    return x0;
  }
}  // namespace feature_generation
