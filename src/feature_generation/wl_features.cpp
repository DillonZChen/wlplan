#include "../../include/feature_generation/wl_features.hpp"

#include "../../include/graph/graph_generator_factory.hpp"
#include "../../include/utils/nlohmann/json.hpp"

#include <fstream>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using json = nlohmann::json;

namespace feature_generation {
  WLFeatures::WLFeatures(const planning::Domain &domain,
                         std::string graph_representation,
                         int iterations,
                         std::string prune_features,
                         bool multiset_hash)
      : package_version(MACRO_STRINGIFY(WLPLAN_VERSION)),
        graph_representation(graph_representation),
        iterations(iterations),
        prune_features(prune_features),
        multiset_hash(multiset_hash) {
    this->domain = std::make_shared<planning::Domain>(domain);
    graph_generator = graph::create_graph_generator(graph_representation, domain);
    collected = false;
    collecting = false;
    neighbour_container = std::make_shared<NeighbourContainer>(multiset_hash);
    seen_colour_statistics = std::vector<std::vector<long>>(2, std::vector<long>(iterations, 0));
    store_weights = false;
  }

  WLFeatures::WLFeatures(const std::string &filename) {
    // let Python handle file exceptions
    std::cout << "Loading feature generator from " << filename << " ..." << std::endl;
    std::ifstream i(filename);
    json j;
    i >> j;
    std::string current_package_version = MACRO_STRINGIFY(WLPLAN_VERSION);

    // load configurations
    package_version = j["package_version"];
    if (package_version != current_package_version) {
      std::cout << "WARNING: loaded generator was created with version " << package_version
                << " but current version is " << current_package_version;
      std::cout << "This may lead to unexpected behaviour." << std::endl;
    }
    graph_representation = j.at("graph_representation").get<std::string>();
    iterations = j.at("iterations").get<int>();
    prune_features = j.at("prune_features").get<std::string>();
    multiset_hash = j.at("multiset_hash").get<bool>();

    std::cout << "package_version=" << package_version << std::endl;
    std::cout << "graph_representation=" << graph_representation << std::endl;
    std::cout << "iterations=" << iterations << std::endl;
    std::cout << "prune_features=" << prune_features << std::endl;
    std::cout << "multiset_hash=" << multiset_hash << std::endl;

    // load colours
    colour_hash = j.at("colour_hash").get<std::unordered_map<std::string, int>>();
    colour_to_layer = j.at("colour_to_layer").get<std::vector<int>>();
    colours_to_keep = j.at("colours_to_keep").get<std::vector<int>>();

    // initialise domain object
    std::string domain_name = j.at("domain").at("name").get<std::string>();
    std::vector<std::pair<std::string, int>> raw_predicates =
        j.at("domain").at("predicates").get<std::vector<std::pair<std::string, int>>>();
    std::vector<planning::Predicate> domain_predicates = std::vector<planning::Predicate>();
    for (size_t i = 0; i < raw_predicates.size(); i++) {
      domain_predicates.push_back(
          planning::Predicate(raw_predicates[i].first, raw_predicates[i].second));
    }
    std::vector<planning::Object> constant_objects =
        j.at("domain").at("constant_objects").get<std::vector<planning::Object>>();

    // load weights if they exist
    std::vector<double> weights_tmp = j.at("weights").get<std::vector<double>>();
    std::cout << "weights_size=" << weights_tmp.size() << std::endl;
    if (weights_tmp.size() > 0) {
      store_weights = true;
      weights = weights_tmp;
    } else {
      store_weights = false;
    }

    // initialise other variables
    domain = std::make_shared<planning::Domain>(domain_name, domain_predicates, constant_objects);
    graph_generator = graph::create_graph_generator(graph_representation, *domain);
    collected = true;  // assume generator already collected colours
    collecting = false;
    neighbour_container = std::make_shared<NeighbourContainer>(multiset_hash);
    seen_colour_statistics = std::vector<std::vector<long>>(2, std::vector<long>(iterations, 0));

    std::cout << "Feature generator loaded!" << std::endl;
  }

  /* feature functions */

  int WLFeatures::get_colour_hash(const std::string &colour) {
    if (!collecting && colour_hash.count(colour) == 0) {
      return -1;
    } else if (collecting && colour_hash.count(colour) == 0) {
      int hash = (int)colour_hash.size();
      colour_hash[colour] = hash;
      colour_to_layer.push_back(cur_collecting_layer);
    }
    return colour_hash[colour];
  }

  void WLFeatures::refine(const graph::Graph &graph,
                          std::vector<int> &colours,
                          std::vector<int> &colours_tmp) {
    // memory for storing string and hashed int representation of colours
    std::string new_colour;
    int new_colour_compressed;

    for (size_t u = 0; u < graph.nodes.size(); u++) {
      // skip unseen colours
      if (colours[u] == -1) {
        new_colour_compressed = -1;
        goto end_of_iteration;
      }
      neighbour_container->clear();

      for (const auto &edge : graph.edges[u]) {
        // skip unseen colours
        if (colours[edge.second] == -1) {
          new_colour_compressed = -1;
          goto end_of_iteration;
        }

        // add sorted neighbour (colour, edge_label) pair
        neighbour_container->insert(colours[edge.second], edge.first);
      }

      // add current colour and sorted neighbours into sorted colour key
      new_colour = std::to_string(colours[u]) + neighbour_container->to_string();

      // hash seen colours
      new_colour_compressed = get_colour_hash(new_colour);

    end_of_iteration:
      colours_tmp[u] = new_colour_compressed;
    }

    colours.swap(colours_tmp);
  }

  std::vector<graph::Graph> WLFeatures::convert_to_graphs(const data::Dataset dataset) {
    std::vector<graph::Graph> graphs;

    const std::vector<data::ProblemStates> &data = dataset.data;
    for (size_t i = 0; i < data.size(); i++) {
      const auto &problem_states = data[i];
      const auto &problem = problem_states.problem;
      const auto &states = problem_states.states;
      graph_generator->set_problem(problem);
      for (const planning::State &state : states) {
        graphs.push_back(*(graph_generator->to_graph(state)));
      }
    }

    return graphs;
  }

  void WLFeatures::collect(const data::Dataset dataset) {

    /* 0. throw error if not using an implemented graph generator */
    if (graph_generator == nullptr) {
      std::string err_msg = "No graph generator is set. Use graph input instead of dataset.";
      throw std::runtime_error(err_msg);
    }

    /* 1. convert dataset to graphs */
    std::vector<graph::Graph> graphs = convert_to_graphs(dataset);

    /* 2. collect colours */
    collect(graphs);
  }

  void WLFeatures::collect(const std::vector<graph::Graph> &graphs) {
    collecting = true;

    // training embeddings for graphs
    std::vector<std::unordered_map<int, int>> graph_histograms;

    // intermediate graph colours during WL
    std::vector<std::vector<int>> graph_colours;

    // extra memory for WL updates
    std::vector<std::vector<int>> graph_colours_tmp;

    // 2.1 init data structures and collect initial colours
    for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
      const auto &graph = graphs[graph_i];
      std::unordered_map<int, int> histogram;
      int n_nodes = graph.nodes.size();
      std::vector<int> colours(n_nodes);
      for (int node_i = 0; node_i < n_nodes; node_i++) {
        cur_collecting_layer = 0;
        int col = get_colour_hash(std::to_string(graph.nodes[node_i]));
        if (histogram.count(col) == 0) {
          histogram[col] = 0;
        }
        histogram[col]++;
        colours[node_i] = col;
      }
      graph_histograms.push_back(histogram);
      graph_colours.push_back(colours);
      graph_colours_tmp.push_back(colours);
    }

    // 2.2 Main WL loop
    for (int iteration = 1; iteration < iterations + 1; iteration++) {
      int colours_collected = get_n_features();
      cur_collecting_layer = iteration;
      for (size_t graph_i = 0; graph_i < graphs.size(); graph_i++) {
        refine(graphs[graph_i], graph_colours[graph_i], graph_colours_tmp[graph_i]);

        for (const int colour : graph_colours[graph_i]) {
          assert(colour != -1);
          if (graph_histograms[graph_i].count(colour) == 0) {
            graph_histograms[graph_i][colour] = 0;
          }
          graph_histograms[graph_i][colour]++;
        }
      }
      colours_collected = get_n_features() - colours_collected;
    }

    collected = true;
    collecting = false;

    // 2.3 post processing to detect equivalent features based on final X
    if (prune_features == "no_prune") {
      // do not detect equivalent features
      colours_to_keep = std::vector<int>(colour_hash.size());
      std::iota(colours_to_keep.begin(), colours_to_keep.end(), 0);
      return;
    }

    // want to call embed without invoking collapse features
    std::string prune_features_tmp = prune_features;
    prune_features = "no_prune";
    colours_to_keep = std::vector<int>();
    std::vector<Embedding> X = embed(graphs);
    if (prune_features_tmp == "collapse") {
      std::set<std::vector<int>> unique_columns;
      for (int i = 0; i < (int)colour_hash.size(); i++) {
        std::vector<int> column;
        for (const auto &x : X) {
          column.push_back(x[i]);
        }
        if (unique_columns.count(column) == 0) {
          unique_columns.insert(column);
          colours_to_keep.push_back(i);
        }
      }
    } else if (prune_features_tmp == "collapse_by_layer") {
      std::vector<std::set<std::vector<int>>> unique_columns(iterations + 1);
      for (int i = 0; i < (int)colour_hash.size(); i++) {
        std::vector<int> column;
        for (const auto &x : X) {
          column.push_back(x[i]);
        }
        int layer = colour_to_layer[i];
        if (unique_columns[layer].count(column) == 0) {
          unique_columns[layer].insert(column);
          colours_to_keep.push_back(i);
        }
      }
    }
    prune_features = prune_features_tmp;

    // check features have been collected
    if (get_n_features() == 0) {
      throw std::runtime_error("No features have been collected.");
    }
  }

  void WLFeatures::set_problem(const planning::Problem &problem) {
    if (graph_generator != nullptr) {
      graph_generator->set_problem(problem);
    }
  }

  std::vector<Embedding> WLFeatures::embed(const data::Dataset &dataset) {
    std::vector<graph::Graph> graphs = convert_to_graphs(dataset);
    if (graphs.size() == 0) {
      throw std::runtime_error("No graphs to embed");
    }
    return embed(graphs);
  }

  std::vector<Embedding> WLFeatures::embed(const std::vector<graph::Graph> &graphs) {
    std::vector<Embedding> X;
    for (const auto &graph : graphs) {
      X.push_back(embed(graph));
    }
    return X;
  }

  Embedding WLFeatures::embed(const graph::Graph &graph) {
    collecting = false;
    if (!collected) {
      throw std::runtime_error("WLFeatures::collect() must be called before embedding");
    }

    /* 1. Initialise embedding before pruning */
    Embedding x0(colour_hash.size(), 0);

    /* 2. Set up memory for WL updates */
    int n_nodes = graph.nodes.size();
    std::vector<int> colours(n_nodes);
    std::vector<int> colours_tmp(n_nodes);

    /* 3. Compute initial colours */
    for (int node_i = 0; node_i < n_nodes; node_i++) {
      int col = get_colour_hash(std::to_string(graph.nodes[node_i]));
      colours[node_i] = col;
      x0[col]++;
    }

    /* 4. Main WL loop */
    int is_seen_colour;
    for (int itr = 0; itr < iterations; itr++) {
      refine(graph, colours, colours_tmp);
      for (const int col : colours) {
        is_seen_colour = (col != -1);  // prevent branch prediction
        x0[col] += is_seen_colour;
        seen_colour_statistics[is_seen_colour][itr]++;
      }
    }

    /* 5. Prune features with colours_to_keep */
    if (prune_features == "no_prune") {
      return x0;
    }

    Embedding x(colours_to_keep.size(), 0);
    for (size_t i = 0; i < colours_to_keep.size(); i++) {
      x[i] = x0[colours_to_keep[i]];
    }
    return x;
  }

  Embedding WLFeatures::embed(const planning::State &state) {
    graph::Graph graph = *(graph_generator->to_graph(state));
    return embed(graph);
  }

  /* prediction functions */

  void WLFeatures::set_weights(const std::vector<double> &weights) {
    if (((int)weights.size()) != get_n_features()) {
      throw std::runtime_error("Number of weights must match number of features.");
    }
    store_weights = true;
    this->weights = weights;
  }

  std::vector<double> WLFeatures::get_weights() const {
    if (!store_weights) {
      throw std::runtime_error("Cannot get weights as they are not stored.");
    }
    return weights;
  }

  double WLFeatures::predict(const graph::Graph &graph) {
    if (!store_weights) {
      throw std::runtime_error("Weights have not been set for prediction.");
    }

    Embedding x = embed(graph);
    double h = std::inner_product(x.begin(), x.end(), weights.begin(), 0.0);
    return h;
  }

  double WLFeatures::predict(const planning::State &state) {
    graph::Graph graph = *(graph_generator->to_graph(state));
    return predict(graph);
  }

  /* I/O functions */

  void WLFeatures::save(const std::string &filename) {
    // let Python handle file exceptions
    json j;
    j["package_version"] = package_version;
    j["graph_representation"] = graph_representation;
    j["iterations"] = iterations;
    j["prune_features"] = prune_features;
    j["multiset_hash"] = multiset_hash;

    j["domain"] = domain->to_json();

    j["colour_hash"] = colour_hash;
    j["colour_to_layer"] = colour_to_layer;
    j["colours_to_keep"] = colours_to_keep;

    j["weights"] = weights;

    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
  }
}  // namespace feature_generation
