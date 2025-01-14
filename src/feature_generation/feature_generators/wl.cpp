#include "../../../include/feature_generation/feature_generators/wl.hpp"

#include "../../../include/feature_generation/dependency_graph.hpp"
#include "../../../include/graph/graph_generator_factory.hpp"
#include "../../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <queue>
#include <sstream>

using json = nlohmann::json;

const int DISTINCT = -1;

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

  inline void log_feature_info(std::vector<int> feature_group, std::vector<int> group_size) {
    int n_distinct_features = 0;
    int n_equivalent_features = 0;
    int n_equivalence_groups = 0;
    for (const int group : feature_group) {
      if (group == DISTINCT) {
        n_distinct_features++;
      } else if (group_size.at(group) > 1) {
        n_equivalent_features++;
      } else {  // (group_size.at(group) <= 1)
        std::cout << "error: equivalence group has a distinct feature" << std::endl;
        exit(-1);
      }
    }
    for (const int size : group_size) {
      if (size > 1) {
        n_equivalence_groups++;
      }
    }
    std::cout << "  Distinct features: " << n_distinct_features << std::endl;
    std::cout << "  Equivalent features: " << n_equivalent_features << std::endl;
    std::cout << "  Equivalence groups: " << n_equivalence_groups << std::endl;
    std::cout << "  Lower bound features: " << n_distinct_features + n_equivalence_groups << std::endl;
  }

  void WLFeatures::collect_impl(const std::vector<graph::Graph> &graphs) {
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

      /* COLLAPSE_LAYER: remove duplicate features greedily every iteration */
      if (pruning == PruningOptions::COLLAPSE_LAYER) {
        collapse_layer_pruning(iteration, graph_colours);
      }
    }

    if (pruning == PruningOptions::COLLAPSE_LAYER) {
      collapse_layer_redundancy_check();
    } else if (pruning == PruningOptions::COLLAPSE_ALL) {
      std::cout << "Collapsing equivalent features..." << std::endl;
      int n_features = get_n_features();
      FeatureDependencyGraph fdg = FeatureDependencyGraph(colour_hash);

      // 1. compute equivalent features candidates
      std::cout << "Computing equivalent feature candidates." << std::endl;
      collected = true;  // required for embed_graphs
      std::vector<Embedding> X = embed_graphs(graphs);
      std::unordered_map<std::vector<int>, int, int_vector_hasher> canonical_group;
      std::vector<int> group_size(n_features, 0);
      std::vector<int> feature_group(n_features, 0);
      for (int colour = 0; colour < n_features; colour++) {
        std::vector<int> feature;
        for (size_t j = 0; j < X.size(); j++) {
          feature.push_back(int(std::round(X[j][colour])));
        }
        int group;
        if (canonical_group.count(feature) == 0) {
          group = canonical_group.size();
          canonical_group[feature] = group;
        } else {  // seen this feature before
          group = canonical_group.at(feature);
        }
        group_size.at(group)++;
        feature_group.at(colour) = group;
      }

      // mark distinct features
      for (int colour = 0; colour < n_features; colour++) {
        int group = feature_group.at(colour);
        if (group >= 0 && group_size.at(group) <= 1) {
          feature_group[colour] = DISTINCT;
        }
      }
      log_feature_info(feature_group, group_size);

      // 2. mark features that cannot be thrown out from highest iteration down
      std::cout << "Pruning features via dependency graph." << std::endl;
      for (int itr = iterations; itr >= 0; itr--) {
        for (const int colour : layer_to_colours.at(itr)) {
          if (feature_group.at(colour) != DISTINCT) {
            continue;
          }
          for (const int ancestor_colour : fdg.get_bw_edges(colour)) {
            int ancestor_group = feature_group.at(ancestor_colour);
            feature_group.at(ancestor_colour) = DISTINCT;
            if (ancestor_group == DISTINCT) {
              continue;
            }
            group_size.at(ancestor_group)--;
          }
        }
      }

      // remark distinct features
      for (int colour = 0; colour < n_features; colour++) {
        int group = feature_group.at(colour);
        if (group >= 0 && group_size.at(group) <= 1) {
          feature_group[colour] = DISTINCT;
        }
      }
      log_feature_info(feature_group, group_size);

      // 3. maxsat
      // TODO
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
