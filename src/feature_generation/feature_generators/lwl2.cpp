#include "../../../include/feature_generation/feature_generators/lwl2.hpp"

#include "../../../include/graph/graph_generator_factory.hpp"
#include "../../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace feature_generation {
  LWL2Features::LWL2Features(const planning::Domain &domain,
                             std::string graph_representation,
                             int iterations,
                             std::string pruning,
                             bool multiset_hash)
      : KWL2Features("2-lwl", domain, graph_representation, iterations, pruning, multiset_hash) {}

  LWL2Features::LWL2Features(const std::string &filename) : KWL2Features(filename) {}

  int lwl2_pair_to_index_map(int n, int i, int j) {
    // map pair where 0 <= i < j < n to vec index
    return j - i - 1 + (i * n) - (i * (i + 1)) / 2;
  }

  int get_n_lwl2_pairs(int n_nodes) { return static_cast<int>((n_nodes * (n_nodes - 1)) / 2); }

  void LWL2Features::refine(const std::shared_ptr<graph::Graph> &graph,
                            std::vector<std::set<int>> &pair_to_neighbours,
                            std::vector<int> &colours,
                            int iteration) {
    // memory for storing string and hashed int representation of colours
    std::vector<int> new_colour;
    std::vector<int> neighbour_vector;
    int new_colour_compressed, pair1, pair2, pair1_col, pair2_col;
    int n_nodes = graph->nodes.size();

    std::vector<int> new_colours(colours.size(), UNSEEN_COLOUR);

    for (int u = 0; u < n_nodes; u++) {
      for (int v = u + 1; v < n_nodes; v++) {
        int index = lwl2_pair_to_index_map(n_nodes, u, v);
        if (colours[index] == UNSEEN_COLOUR) {
          new_colour_compressed = UNSEEN_COLOUR;
          goto end_of_iteration;
        }

        neighbour_container->clear();

        for (const int w : pair_to_neighbours[index]) {
          if (u < w) {
            pair1 = lwl2_pair_to_index_map(n_nodes, u, w);
          } else {
            pair1 = lwl2_pair_to_index_map(n_nodes, w, u);
          }
          if (v < w) {
            pair2 = lwl2_pair_to_index_map(n_nodes, v, w);
          } else {
            pair2 = lwl2_pair_to_index_map(n_nodes, w, v);
          }
          pair1_col = colours[pair1];
          pair2_col = colours[pair2];
          if (pair1_col == UNSEEN_COLOUR || pair2_col == UNSEEN_COLOUR) {
            new_colour_compressed = UNSEEN_COLOUR;
            goto end_of_iteration;
          }
          // min max used because of sets
          neighbour_container->insert(std::min(pair1_col, pair2_col),
                                      std::max(pair1_col, pair2_col));
        }

        // add current colour and sorted neighbours into sorted colour key
        new_colour = {colours[index]};
        neighbour_vector = neighbour_container->to_vector();

        new_colour.insert(new_colour.end(), neighbour_vector.begin(), neighbour_vector.end());

        // hash seen colours
        new_colour_compressed = get_colour_hash(new_colour, iteration);

      end_of_iteration:
        new_colours[index] = new_colour_compressed;
      }
    }

    colours = new_colours;
  }

  std::vector<int> get_lwl2_pair_to_edge_label(std::shared_ptr<graph::Graph> graph) {
    int n_nodes = graph->nodes.size();
    int n_pairs = get_n_lwl2_pairs(n_nodes);
    std::vector<int> pair_to_edge_label(n_pairs, NO_EDGE_COLOUR);
    for (int u = 0; u < n_nodes; u++) {
      for (const auto &[v, edge_label] : graph->edges[u]) {
        if (u < v) {
          pair_to_edge_label[lwl2_pair_to_index_map(n_nodes, u, v)] = edge_label;
        }
      }
    }
    return pair_to_edge_label;
  }

  std::vector<std::set<int>> get_lwl2_pair_to_neighbours(std::shared_ptr<graph::Graph> graph) {
    int n_nodes = graph->nodes.size();
    int n_pairs = get_n_lwl2_pairs(n_nodes);
    std::vector<std::set<int>> node_to_neighbours = graph->get_node_to_neighbours();
    std::vector<std::set<int>> pair_to_neighbours(n_pairs, std::set<int>());
    for (int u = 0; u < n_nodes; u++) {
      for (int v = u + 1; v < n_nodes; v++) {
        int index = lwl2_pair_to_index_map(n_nodes, u, v);
        pair_to_neighbours[index] = node_to_neighbours[u];
        pair_to_neighbours[index].insert(node_to_neighbours[v].cbegin(),
                                         node_to_neighbours[v].cend());
        pair_to_neighbours[index].erase(u);
        pair_to_neighbours[index].erase(v);
      }
    }
    return pair_to_neighbours;
  }

  int LWL2Features::get_initial_colour(int index,
                                       int u,
                                       int v,
                                       const std::shared_ptr<graph::Graph> &graph,
                                       const std::vector<int> &pair_to_edge_label) {
    int u_col = graph->nodes[u];
    int v_col = graph->nodes[v];
    int e_col = pair_to_edge_label[index];
    int col = get_colour_hash({std::min(u_col, v_col), std::max(u_col, v_col), e_col}, 0);
    return col;
  }

  void LWL2Features::collect_impl(const std::vector<graph::Graph> &graphs) {
    // intermediate graph colours during WL
    std::vector<std::vector<int>> graph_colours;

    // init colours
    log_iteration(0);
    for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
      const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
      int n_nodes = graph->nodes.size();
      int n_pairs = get_n_lwl2_pairs(n_nodes);

      std::vector<int> colours(n_pairs, 0);

      std::vector<int> pair_to_edge_label = get_lwl2_pair_to_edge_label(graph);
      std::vector<std::set<int>> pair_to_neighbours = get_lwl2_pair_to_neighbours(graph);

      // init colours
      for (int u = 0; u < n_nodes; u++) {
        for (int v = u + 1; v < n_nodes; v++) {
          int index = lwl2_pair_to_index_map(n_nodes, u, v);
          int col = get_initial_colour(index, u, v, graph, pair_to_edge_label);
          colours[index] = col;
        }
      }

      graph_colours.push_back(colours);
    }

    // main WL loop
    for (int itr = 1; itr < iterations + 1; itr++) {
      log_iteration(itr);
      for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
        const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
        std::vector<std::set<int>> pair_to_neighbours = get_lwl2_pair_to_neighbours(graph);
        refine(graph, pair_to_neighbours, graph_colours[graph_i], itr);
      }

      // layer pruning
      prune_this_iteration(itr, graphs, graph_colours);
    }
  }

  Embedding LWL2Features::embed_impl(const std::shared_ptr<graph::Graph> &graph) {
    /* 1. Initialise embedding before pruning */
    Embedding x0(get_n_features(), 0);

    int n_nodes = graph->nodes.size();
    int n_pairs = get_n_lwl2_pairs(n_nodes);
    std::vector<int> colours(n_pairs);

    std::vector<int> pair_to_edge_label = get_lwl2_pair_to_edge_label(graph);
    std::vector<std::set<int>> pair_to_neighbours = get_lwl2_pair_to_neighbours(graph);

    /* 2. Compute initial colours */
    for (int u = 0; u < n_nodes; u++) {
      for (int v = u + 1; v < n_nodes; v++) {
        int index = lwl2_pair_to_index_map(n_nodes, u, v);
        int col = get_initial_colour(index, u, v, graph, pair_to_edge_label);
        colours[index] = col;
        add_colour_to_x(col, 0, x0);
      }
    }

    /* 3. Main WL loop */
    for (int itr = 1; itr < iterations + 1; itr++) {
      refine(graph, pair_to_neighbours, colours, itr);
      for (const int col : colours) {
        add_colour_to_x(col, itr, x0);
      }
    }

    return x0;
  }
}  // namespace feature_generation
