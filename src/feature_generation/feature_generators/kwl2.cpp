#include "../../../include/feature_generation/feature_generators/kwl2.hpp"

#include "../../../include/graph/graph_generator_factory.hpp"
#include "../../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace feature_generation {
  KWL2Features::KWL2Features(const std::string wl_name,
                             const planning::Domain &domain,
                             std::string graph_representation,
                             int iterations,
                             std::string pruning,
                             bool multiset_hash)
      : Features(wl_name, domain, graph_representation, iterations, pruning, multiset_hash) {}

  KWL2Features::KWL2Features(const planning::Domain &domain,
                             std::string graph_representation,
                             int iterations,
                             std::string pruning,
                             bool multiset_hash)
      : Features("2-kwl", domain, graph_representation, iterations, pruning, multiset_hash) {}

  KWL2Features::KWL2Features(const std::string &filename) : Features(filename) {}

  std::vector<int> KWL2Features::reformat_neighbour_colours(const std::vector<int> &colours,
                                                            const std::map<int, int> &remap) {
    std::cout << "not implemented yet" << std::endl;
    exit(-1);
    return std::vector<int>();
  }

  int kwl2_pair_to_index_map(int n, int i, int j) {
    // map pair where 0 <= i, j < n to vec index
    return i * n + j;
  }

  int get_n_kwl2_pairs(int n_nodes) { return static_cast<int>(n_nodes * n_nodes); }

  void KWL2Features::refine(const std::shared_ptr<graph::Graph> &graph,
                            std::vector<int> &colours,
                            std::vector<int> &colours_tmp) {
    // memory for storing string and hashed int representation of colours
    std::vector<int> new_colour;
    std::vector<int> neighbour_vector;
    int new_colour_compressed, pair1, pair2, pair1_col, pair2_col;
    int n_nodes = graph->nodes.size();

    for (int u = 0; u < n_nodes; u++) {
      for (int v = 0; v < n_nodes; v++) {
        int index = kwl2_pair_to_index_map(n_nodes, u, v);
        if (colours[index] == UNSEEN_COLOUR) {
          new_colour_compressed = UNSEEN_COLOUR;
          goto end_of_iteration;
        }

        neighbour_container->clear();

        // original kWL iterates over all nodes for neighbours
        for (int w = 0; w < n_nodes; w++) {
          pair1 = kwl2_pair_to_index_map(n_nodes, u, w);
          pair2 = kwl2_pair_to_index_map(n_nodes, w, v);
          pair1_col = colours[pair1];
          pair2_col = colours[pair2];
          if (pair1_col == UNSEEN_COLOUR || pair2_col == UNSEEN_COLOUR) {
            new_colour_compressed = UNSEEN_COLOUR;
            goto end_of_iteration;
          }
          neighbour_container->insert(pair1_col, pair2_col);
        }

        // add current colour and sorted neighbours into sorted colour key
        new_colour = {colours[index]};
        neighbour_vector = neighbour_container->to_vector();

        new_colour.insert(new_colour.end(), neighbour_vector.begin(), neighbour_vector.end());

        // hash seen colours
        new_colour_compressed = get_colour_hash(new_colour);

      end_of_iteration:
        colours_tmp[index] = new_colour_compressed;
      }
    }

    colours.swap(colours_tmp);
  }

  std::vector<int> get_kwl2_pair_to_edge_label(std::shared_ptr<graph::Graph> graph) {
    int n_nodes = graph->nodes.size();
    int n_pairs = get_n_kwl2_pairs(n_nodes);
    std::vector<int> pair_to_edge_label(n_pairs, NO_EDGE_COLOUR);
    for (int u = 0; u < n_nodes; u++) {
      for (const auto &[v, edge_label] : graph->edges[u]) {
        pair_to_edge_label[kwl2_pair_to_index_map(n_nodes, u, v)] = edge_label;
        pair_to_edge_label[kwl2_pair_to_index_map(n_nodes, v, u)] = edge_label;
      }
    }
    return pair_to_edge_label;
  }

  int KWL2Features::get_initial_colour(int index,
                                       int u,
                                       int v,
                                       const std::shared_ptr<graph::Graph> &graph,
                                       const std::vector<int> &pair_to_edge_label) {
    int u_col = graph->nodes[u];
    int v_col = graph->nodes[v];
    int e_col = pair_to_edge_label[index];
    int col = get_colour_hash({u_col, v_col, e_col});
    return col;
  }

  void KWL2Features::collect_impl(const std::vector<graph::Graph> &graphs) {
    // intermediate graph colours during WL and extra memory for WL updates
    std::vector<int> colours;
    std::vector<int> colours_tmp;

    n_seen_graphs += graphs.size();
    for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
      const auto graph = std::make_shared<graph::Graph>(graphs[graph_i]);
      auto edges = graph->edges;
      int n_nodes = graph->nodes.size();
      int n_edges = graph->get_n_edges();
      n_seen_nodes += n_nodes;
      n_seen_edges += n_edges;

      int n_pairs = get_n_kwl2_pairs(n_nodes);

      // intermediate colours
      colours = std::vector<int>(n_pairs, 0);
      colours_tmp = std::vector<int>(n_pairs, 0);

      std::vector<int> pair_to_edge_label = get_kwl2_pair_to_edge_label(graph);

      // init colours
      cur_collecting_layer = 0;
      for (int u = 0; u < n_nodes; u++) {
        for (int v = 0; v < n_nodes; v++) {
          int index = kwl2_pair_to_index_map(n_nodes, u, v);
          int col = get_initial_colour(index, u, v, graph, pair_to_edge_label);
          colours[index] = col;
          seen_initial_colours.insert(col);
        }
      }

      // main WL loop
      for (int iteration = 1; iteration < iterations + 1; iteration++) {
        cur_collecting_layer = iteration;
        refine(graph, colours, colours_tmp);
      }
    }
  }

  Embedding KWL2Features::embed(const std::shared_ptr<graph::Graph> &graph) {
    collecting = false;
    if (!collected) {
      throw std::runtime_error("KWL2Features::collect() must be called before embedding");
    }

    /* 1. Initialise embedding before pruning */
    Embedding x0(colour_hash.size(), 0);

    /* 2. Set up memory for WL updates */
    int n_nodes = graph->nodes.size();
    int n_pairs = get_n_kwl2_pairs(n_nodes);
    std::vector<int> colours(n_pairs);
    std::vector<int> colours_tmp(n_pairs);

    std::vector<int> pair_to_edge_label = get_kwl2_pair_to_edge_label(graph);

    /* 3. Compute initial colours */
    for (int u = 0; u < n_nodes; u++) {
      for (int v = 0; v < n_nodes; v++) {
        int index = kwl2_pair_to_index_map(n_nodes, u, v);
        int col = get_initial_colour(index, u, v, graph, pair_to_edge_label);
        colours[index] = col;
        if (col != UNSEEN_COLOUR) {
          x0[col]++;
        }
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

    return x0;
  }
}  // namespace feature_generation
