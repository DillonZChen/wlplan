#include "../../../include/feature_generator/feature_generators/iwl.hpp"

#include "../../../include/graph_generator/graph_generator_factory.hpp"
#include "../../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace feature_generator {
  IWLFeatures::IWLFeatures(const std::string feature_name,
                           const planning::Domain &domain,
                           std::string graph_representation,
                           int iterations,
                           std::string pruning,
                           bool multiset_hash)
      : WLFeatures(feature_name, domain, graph_representation, iterations, pruning, multiset_hash) {
  }

  IWLFeatures::IWLFeatures(const planning::Domain &domain,
                           std::string graph_representation,
                           int iterations,
                           std::string pruning,
                           bool multiset_hash)
      : IWLFeatures("iwl", domain, graph_representation, iterations, pruning, multiset_hash) {}

  IWLFeatures::IWLFeatures(const std::string &filename) : WLFeatures(filename) {}

  IWLFeatures::IWLFeatures(const std::string &filename, bool quiet) : WLFeatures(filename, quiet) {}

  void IWLFeatures::refine(const std::shared_ptr<graph_generator::Graph> &graph,
                           std::vector<int> &colours,
                           int iteration) {
    // memory for storing string and hashed int representation of colours
    std::vector<int> new_colour;
    std::vector<int> neighbour_vector;
    int new_colour_compressed;

    std::vector<int> new_colours(colours.size(), UNSEEN_COLOUR);

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
      neighbour_vector = neighbour_container->to_vector();

      new_colour.insert(new_colour.end(), neighbour_vector.begin(), neighbour_vector.end());

      // hash seen colours
      new_colour_compressed = get_colour_hash(new_colour, iteration);

    end_of_iteration:
      new_colours[u] = new_colour_compressed;
    }

    colours = new_colours;
  }

  void IWLFeatures::collect_impl(const std::vector<graph_generator::Graph> &graphs) {
    // intermediate graph colours during WL
    std::vector<int> colours;

    // init colours
    for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
      const auto graph = std::make_shared<graph_generator::Graph>(graphs[graph_i]);
      int n_nodes = graph->nodes.size();

      // individualisation for each node
      for (int node_i = 0; node_i < n_nodes; node_i++) {
        colours = std::vector<int>(n_nodes, 0);

        for (int u = 0; u < n_nodes; u++) {
          std::vector<int> colour_key = {graph->nodes[u]};
          if (u == node_i) {
            colour_key.push_back(INDIVIDUALISE_COLOUR);
          }
          int col = get_colour_hash(colour_key, 0);
          colours[u] = col;
        }

        // main WL loop
        for (int iteration = 1; iteration < iterations + 1; iteration++) {
          refine(graph, colours, iteration);
        }
      }
    }
  }

  Embedding IWLFeatures::embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) {
    /* 1. Initialise embedding */
    Embedding x0(get_n_features(), 0);
    int n_nodes = graph->nodes.size();

    /* Individualisation */
    for (int node_i = 0; node_i < n_nodes; node_i++) {
      std::vector<int> colours(n_nodes);

      /* 2. Compute initial colours */
      for (int u = 0; u < n_nodes; u++) {
        std::vector<int> colour_key = {graph->nodes[u]};
        if (u == node_i) {
          colour_key.push_back(INDIVIDUALISE_COLOUR);
        }
        int col = get_colour_hash(colour_key, 0);
        add_colour_to_x(col, 0, x0);
      }

      /* 3. Main WL loop */
      for (int itr = 1; itr < iterations + 1; itr++) {
        refine(graph, colours, itr);
        for (const int col : colours) {
          add_colour_to_x(col, itr, x0);
        }
      }
    }

    return x0;
  }
}  // namespace feature_generator
