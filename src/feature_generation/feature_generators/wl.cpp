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
                          std::vector<int> &colours_tmp,
                          int iteration) {
    // memory for storing string and hashed int representation of colours
    std::vector<int> new_colour;
    std::vector<int> neighbour_vector;
    int new_colour_compressed;

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
      colours_tmp[u] = new_colour_compressed;
    }

    // discard nodes
    for (const int u : nodes_to_discard) {
      nodes.erase(u);
    }

    colours.swap(colours_tmp);
  }

  void WLFeatures::collect_impl(const std::vector<graph::Graph> &graphs) {
    // intermediate graph colours during WL and extra memory for WL updates
    std::vector<std::vector<int>> graph_colours;
    std::vector<std::vector<int>> graph_colours_tmp;

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
      graph_colours_tmp.push_back(std::vector<int>(n_nodes, 0));
    }

    // main WL loop
    for (int itr = 1; itr < iterations + 1; itr++) {
      log_iteration(itr);
      for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
        const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
        std::set<int> nodes = graph->get_nodes_set();
        refine(graph, nodes, graph_colours[graph_i], graph_colours_tmp[graph_i], itr);
      }

      // layer pruning
      prune_this_iteration(itr, graphs, graph_colours);
    }
  }

  Embedding WLFeatures::embed(const std::shared_ptr<graph::Graph> &graph) {
    collecting = false;
    if (!collected) {
      throw std::runtime_error("WLFeatures::collect() must be called before embedding");
    }

    /* 1. Initialise embedding before pruning, and set up memory */
    Embedding x0(get_n_features(), 0);
    int n_nodes = graph->nodes.size();
    std::vector<int> colours(n_nodes);
    std::vector<int> colours_tmp(n_nodes);
    std::set<int> nodes = graph->get_nodes_set();

    /* 2. Compute initial colours */
    for (const int node_i : nodes) {
      int col = get_colour_hash({graph->nodes[node_i]}, 0);
      colours[node_i] = col;
      add_colour_to_x(col, 0, x0);
    }

    /* 3. Main WL loop */
    for (int itr = 1; itr < iterations + 1; itr++) {
      refine(graph, nodes, colours, colours_tmp, itr);
      for (const int col : colours) {
        add_colour_to_x(col, itr, x0);
      }
    }

    return x0;
  }
}  // namespace feature_generation
