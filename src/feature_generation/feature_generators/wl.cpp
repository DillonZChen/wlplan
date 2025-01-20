#include "../../../include/feature_generation/feature_generators/wl.hpp"

#include "../../../include/graph/graph_generator_factory.hpp"
#include "../../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <queue>
#include <set>
#include <sstream>

using json = nlohmann::json;

namespace feature_generation {
  WLFeatures::WLFeatures(const std::string wl_name,
                         const planning::Domain &domain,
                         std::string graph_representation,
                         int iterations,
                         std::string pruning,
                         bool multiset_hash)
      : Features(wl_name, domain, graph_representation, iterations, pruning, multiset_hash) {}

  WLFeatures::WLFeatures(const planning::Domain &domain,
                         std::string graph_representation,
                         int iterations,
                         std::string pruning,
                         bool multiset_hash)
      : Features("wl", domain, graph_representation, iterations, pruning, multiset_hash) {}

  WLFeatures::WLFeatures(const std::string &filename) : Features(filename) {}

  void WLFeatures::refine(const std::shared_ptr<graph::Graph> &graph,
                          std::set<int> &nodes,
                          std::vector<int> &colours,
                          int iteration) {
    // memory for storing string and hashed int representation of colours
    std::vector<int> new_colour;
    std::vector<int> neighbour_vector;
    int new_colour_compressed;

    std::vector<int> new_colours(colours.size(), UNSEEN_COLOUR);
    std::vector<int> nodes_to_discard;

    for (const int u : nodes) {
      // skip unseen colours
      int current_colour = colours[u];
      if (current_colour == UNSEEN_COLOUR) {
        new_colour_compressed = UNSEEN_COLOUR;
        nodes_to_discard.push_back(u);
        goto end_of_iteration;
      }
      neighbour_container->clear();

      for (const auto &edge : graph->edges[u]) {
        // skip unseen colours
        int neighbour_colour = colours[edge.second];
        if (neighbour_colour == UNSEEN_COLOUR) {
          new_colour_compressed = UNSEEN_COLOUR;
          nodes_to_discard.push_back(u);
          goto end_of_iteration;
        }

        // add sorted neighbour (colour, edge_label) pair
        neighbour_container->insert(neighbour_colour, edge.first);
      }

      // add current colour and sorted neighbours into sorted colour key
      new_colour = {current_colour};
      neighbour_vector = neighbour_container->to_vector();

      new_colour.insert(new_colour.end(), neighbour_vector.begin(), neighbour_vector.end());

      // hash seen colours
      new_colour_compressed = get_colour_hash(new_colour, iteration);

    end_of_iteration:
      new_colours[u] = new_colour_compressed;
    }

    // discard nodes
    for (const int u : nodes_to_discard) {
      nodes.erase(u);
    }

    colours = new_colours;
  }

  void WLFeatures::collect_impl(const std::vector<graph::Graph> &graphs) {
    // Intermediate graph colours during WL
    // It could be more optimal to use map<int, int> for graph colours, with UNSEEN_COLOUR
    // nodes not showing up in the map. However, this would make the code more complex.
    std::vector<std::vector<int>> graph_colours;

    // init colours
    log_iteration(0);
    for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
      const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
      int n_nodes = graph->nodes.size();

      std::vector<int> colours(n_nodes, 0);
      for (int node_i = 0; node_i < n_nodes; node_i++) {
        int col = get_colour_hash({graph->nodes[node_i]}, 0);
        colours[node_i] = col;
      }
      graph_colours.push_back(colours);
    }

    // main WL loop
    for (int itr = 1; itr < iterations + 1; itr++) {
      log_iteration(itr);
      for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
        const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
        std::set<int> nodes = graph->get_nodes_set();
        refine(graph, nodes, graph_colours[graph_i], itr);
      }

      // layer pruning
      prune_this_iteration(itr, graphs, graph_colours);
    }
  }

  Embedding WLFeatures::embed_impl(const std::shared_ptr<graph::Graph> &graph) {
    /* 1. Initialise embedding before pruning, and set up memory */
    Embedding x0(get_n_features(), 0);
    int n_nodes = graph->nodes.size();
    std::vector<int> colours(n_nodes);
    std::set<int> nodes = graph->get_nodes_set();

    /* 2. Compute initial colours */
    for (const int node_i : nodes) {
      int col = get_colour_hash({graph->nodes[node_i]}, 0);
      colours[node_i] = col;
      add_colour_to_x(col, 0, x0);
    }

    /* 3. Main WL loop */
    for (int itr = 1; itr < iterations + 1; itr++) {
      refine(graph, nodes, colours, itr);
      for (const int col : colours) {
        add_colour_to_x(col, itr, x0);
      }
    }

    return x0;
  }
}  // namespace feature_generation
