#include "../../include/feature_generation/wl_features.hpp"

#include "../../include/graph/graph_generator_factory.hpp"
#include "../../include/utils/nlohmann/json.hpp"

#include <fstream>
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
                          std::vector<int> &colours,
                          std::vector<int> &colours_tmp) {
    // memory for storing string and hashed int representation of colours
    std::vector<int> new_colour;
    std::vector<int> neighbour_vector;
    int new_colour_compressed;

    for (size_t u = 0; u < graph->nodes.size(); u++) {
      // skip unseen colours
      int current_colour = colours[u];
      if (current_colour == UNSEEN_COLOUR) {
        new_colour_compressed = UNSEEN_COLOUR;
        goto end_of_iteration;
      }
      neighbour_container->clear();

      for (const auto &edge : graph->edges[u]) {
        // skip unseen colours
        int neighbour_colour = colours[edge.second];
        if (neighbour_colour == UNSEEN_COLOUR) {
          new_colour_compressed = UNSEEN_COLOUR;
          goto end_of_iteration;
        }

        // add sorted neighbour (colour, edge_label) pair
        neighbour_container->insert(neighbour_colour, edge.first);
      }

      // add current colour and sorted neighbours into sorted colour key
      new_colour = {current_colour};

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

  void WLFeatures::collect_main(const std::vector<graph::Graph> &graphs) {
    // intermediate graph colours during WL and extra memory for WL updates
    std::vector<std::vector<int>> graph_colours;
    std::vector<std::vector<int>> graph_colours_tmp;

    n_seen_graphs += graphs.size();
    for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
      const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
      int n_nodes = graph->nodes.size();
      int n_edges = graph->get_n_edges();
      n_seen_nodes += n_nodes;
      n_seen_edges += n_edges;

      std::vector<int> colours(n_nodes, 0);

      // init colours
      cur_collecting_layer = 0;
      for (int node_i = 0; node_i < n_nodes; node_i++) {
        int col = get_colour_hash({graph->nodes[node_i]});
        colours[node_i] = col;
        seen_initial_colours.insert(col);
      }
      graph_colours.push_back(colours);
      graph_colours_tmp.push_back(std::vector<int>(n_nodes, 0));
    }

    // main WL loop
    for (int iteration = 1; iteration < iterations + 1; iteration++) {
      cur_collecting_layer = iteration;


      for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
        const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
        refine(graph, graph_colours[graph_i], graph_colours_tmp[graph_i]);
      }

      if (pruning == "collapse") {
        // remove duplicate features based on their column

        std::map<int, int> collect_colour_remap;
        std::map<int, std::vector<int>> columns;

        for (int colour : layer_to_colours[iteration]) {
          columns[colour] = std::vector<int>(graphs.size(), 0);
        }
        for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
          for (size_t node_i = 0; node_i < graph_colours[graph_i].size(); node_i++) {
            int colour = graph_colours[graph_i][node_i];
            if (colour_to_layer[colour] != iteration) {
              // this occurs for isolated nodes and would cause seg fault below
              continue;
            }
            columns[colour][graph_i]++;
          }
        }

        // just select the earliest unique features
        int n_colours_prev_itr = colour_hash.size() - layer_to_colours[iteration].size();
        int kept_colours = 0;
        std::unordered_set<std::vector<int>, int_vector_hasher> unique_features;
        for (int colour : layer_to_colours[iteration]) {
          std::vector<int> column = columns[colour];
          if (unique_features.count(column) == 0) {
            unique_features.insert(column);
            collect_colour_remap[colour] = kept_colours + n_colours_prev_itr;
            kept_colours++;
          } else {
            // throw out because not unique
            collect_colour_remap[colour] = UNSEEN_COLOUR;
          }
        }

        // map all current thrown out nodes to UNSEEN_COLOUR in the graphs
        for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
          for (size_t node_i = 0; node_i < graph_colours[graph_i].size(); node_i++) {
            int colour = graph_colours[graph_i][node_i];
            if (collect_colour_remap.count(colour) &&
                collect_colour_remap[colour] == UNSEEN_COLOUR) {
              graph_colours[graph_i][node_i] = UNSEEN_COLOUR;
            }
          }
        }

        reformat_colour_hash(collect_colour_remap);
      }
    }
  }

  Embedding WLFeatures::embed(const std::shared_ptr<graph::Graph> &graph) {
    collecting = false;
    if (!collected) {
      throw std::runtime_error("WLFeatures::collect() must be called before embedding");
    }

    /* 1. Initialise embedding before pruning, and set up memory */
    Embedding x0(colour_hash.size(), 0);
    int n_nodes = graph->nodes.size();
    std::vector<int> colours(n_nodes);
    std::vector<int> colours_tmp(n_nodes);

    /* 2. Compute initial colours */
    for (int node_i = 0; node_i < n_nodes; node_i++) {
      int col = get_colour_hash({graph->nodes[node_i]});
      colours[node_i] = col;
      if (col != UNSEEN_COLOUR) {
        x0[col]++;
      }
    }

    /* 3. Main WL loop */
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

    return x0;
  }
}  // namespace feature_generation
