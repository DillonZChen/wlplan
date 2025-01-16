#include "../../include/feature_generation/features.hpp"

#include "../../include/feature_generation/maxsat.hpp"
#include "../../include/graph/graph_generator_factory.hpp"
#include "../../include/utils/nlohmann/json.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

using json = nlohmann::json;

namespace feature_generation {
  Features::Features(const std::string feature_name,
                     const planning::Domain &domain,
                     std::string graph_representation,
                     int iterations,
                     std::string pruning,
                     bool multiset_hash)
      : package_version(MACRO_STRINGIFY(WLPLAN_VERSION)),
        feature_name(feature_name),
        graph_representation(graph_representation),
        iterations(iterations),
        pruning(pruning),
        multiset_hash(multiset_hash) {
    this->domain = std::make_shared<planning::Domain>(domain);
    graph_generator = graph::create_graph_generator(graph_representation, domain);
    collected = false;
    collapse_pruned = false;
    collecting = false;
    neighbour_container = std::make_shared<NeighbourContainer>(multiset_hash);
    seen_colour_statistics = std::vector<std::vector<long>>(2, std::vector<long>(iterations, 0));
    store_weights = false;

    n_seen_graphs = 0;
    n_seen_nodes = 0;
    n_seen_edges = 0;
    seen_initial_colours = std::set<int>();
    init_layer_to_colours();
  }

  void Features::init_layer_to_colours() {
    // plus 1 because zeroth iteration is also included
    layer_to_colours = std::vector<std::set<int>>(iterations + 1, std::set<int>());
  }

  Features::Features(const std::string &filename) {
    // let Python handle file exceptions
    std::ifstream i(filename);
    json j;
    i >> j;
    std::string cur_pkg_ver = MACRO_STRINGIFY(WLPLAN_VERSION);
    cur_pkg_ver.erase(std::remove(cur_pkg_ver.begin(), cur_pkg_ver.end(), '\"'), cur_pkg_ver.end());

    // load configurations
    package_version = j["package_version"];
    if (package_version != cur_pkg_ver) {
      std::cout << "WARNING: loaded generator was created with version " << package_version
                << " but current version is " << cur_pkg_ver << ". ";
      std::cout << "This may lead to unexpected behaviour." << std::endl;
    }
    feature_name = j.at("feature_name").get<std::string>();
    graph_representation = j.at("graph_representation").get<std::string>();
    iterations = j.at("iterations").get<int>();
    pruning = j.at("pruning").get<std::string>();
    multiset_hash = j.at("multiset_hash").get<bool>();

    std::cout << "package_version=" << package_version << std::endl;
    std::cout << "feature_name=" << feature_name << std::endl;
    std::cout << "graph_representation=" << graph_representation << std::endl;
    std::cout << "iterations=" << iterations << std::endl;
    std::cout << "pruning=" << pruning << std::endl;
    std::cout << "multiset_hash=" << multiset_hash << std::endl;

    // load colours
    std::unordered_map<std::string, int> colour_hash_str =
        j.at("colour_hash").get<std::unordered_map<std::string, int>>();
    colour_hash = str_to_int_colour_hash(colour_hash_str);
    colour_to_layer = j.at("colour_to_layer").get<std::unordered_map<int, int>>();

    // initialise domain object
    std::string domain_name = j.at("domain").at("name").get<std::string>();

    std::vector<std::pair<std::string, int>> raw_predicates =
        j.at("domain").at("predicates").get<std::vector<std::pair<std::string, int>>>();
    std::vector<planning::Predicate> domain_predicates = std::vector<planning::Predicate>();
    for (size_t i = 0; i < raw_predicates.size(); i++) {
      domain_predicates.push_back(
          planning::Predicate(raw_predicates[i].first, raw_predicates[i].second));
    }

    std::vector<std::pair<std::string, int>> raw_functions =
        j.at("domain").at("functions").get<std::vector<std::pair<std::string, int>>>();
    std::vector<planning::Function> domain_functions = std::vector<planning::Function>();
    for (size_t i = 0; i < raw_functions.size(); i++) {
      domain_functions.push_back(
          planning::Function(raw_functions[i].first, raw_functions[i].second));
    }

    std::vector<planning::Object> constant_objects =
        j.at("domain").at("constant_objects").get<std::vector<planning::Object>>();
    domain = std::make_shared<planning::Domain>(
        domain_name, domain_predicates, domain_functions, constant_objects);
    std::cout << "domain=" << domain->to_string() << std::endl;

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
    graph_generator = graph::create_graph_generator(graph_representation, *domain);
    collected = true;  // assume generator already collected colours
    collecting = false;
    neighbour_container = std::make_shared<NeighbourContainer>(multiset_hash);
    seen_colour_statistics = std::vector<std::vector<long>>(2, std::vector<long>(iterations, 0));
  }

  void Features::set_problem(const planning::Problem &problem) {
    if (graph_generator != nullptr) {
      graph_generator->set_problem(problem);
    }
  }

  /* Feature generation functions */

  int Features::get_colour_hash(const std::vector<int> &colour) {
    if (!collecting && colour_hash.count(colour) == 0) {
      return UNSEEN_COLOUR;
    } else if (collecting && colour_hash.count(colour) == 0) {
      int hash = (int)colour_hash.size();
      colour_hash[colour] = hash;
      colour_to_layer[hash] = cur_collecting_layer;
      layer_to_colours[cur_collecting_layer].insert(hash);
    }
    return colour_hash[colour];
  }

  std::vector<int> Features::remap_neighbour_colours(const std::vector<int> &colours,
                                                     const std::map<int, int> &remap) {
    // make new_colours a copy of colours
    std::vector<int> new_colours = colours;

    // debug_vec(colours);  // DEBUG

    // colours should always show up in remap by their construction
    for (const int i : get_neighbour_colour_indices(colours)) {
      new_colours[i] = remap.at(colours[i]);
    }
    return new_colours;
  }

  std::map<int, int> Features::remap_colour_hash(const std::set<int> &to_prune) {
    // remap values
    std::map<int, int> remap;
    std::vector<std::pair<std::vector<int>, int>> new_hash_vec;
    std::unordered_map<int, int> new_colour_layer;

    // layer 0 colours (init colours) should remain consistent
    for (const auto &[key, val] : colour_hash) {
      int layer = colour_to_layer[val];
      if (seen_initial_colours.count(val) == 0) {
        if (layer == 0) {
          std::cout << "error: encountered refined colour with layer = " << layer << std::endl;
          exit(-1);
        }
        continue;
      } else {
        if (layer != 0) {
          std::cout << "error: encountered initial colour with layer = " << layer << std::endl;
          exit(-1);
        }
        // keep the same for initial colours
        new_hash_vec.push_back(std::make_pair(key, val));
        new_colour_layer[val] = layer;
        remap[val] = val;
      }
    }

    // deal with layer 1+ colours
    for (const auto &[key, val] : colour_hash) {
      if (seen_initial_colours.count(val) > 0 || to_prune.count(val) > 0) {
        continue;
      }
      int new_val = (int)new_hash_vec.size();
      remap[val] = new_val;
      new_hash_vec.push_back(std::make_pair(key, new_val));
      new_colour_layer[new_val] = colour_to_layer[val];
    }

    //////////////////////////////////////////
    // DEBUG
    // std::cout << "initial_colours" << std::endl;
    // for (const int i : seen_initial_colours) {
    //   std::cout << "INITIAL " << i << std::endl;
    // }
    // std::cout << "old_hash" << std::endl;
    // for (const auto &[key, val] : colour_hash) {
    //   std::cout << "HASH "; debug_hash(key, val);
    // }
    // std::cout << "to_prune" << std::endl;
    // for (const int i : to_prune) {
    //   std::cout << "PRUNE " << i << std::endl;
    // }
    // std::cout << "remap" << std::endl;
    // for (const auto &[key, val] : remap) {
    //   std::cout << "REMAP " << key << " -> " << val << std::endl;
    // }
    //////////////////////////////////////////

    // remap keys
    ColourHash new_colour_hash;
    for (size_t i = 0; i < new_hash_vec.size(); i++) {
      std::vector<int> key = new_hash_vec[i].first;
      int val = new_hash_vec[i].second;
      if (new_colour_layer[val] > 0) {
        key = remap_neighbour_colours(key, remap);
      }
      new_colour_hash[key] = val;
    }

    // remap hash
    colour_hash = new_colour_hash;

    // remap colours
    colour_to_layer = new_colour_layer;
    init_layer_to_colours();
    for (const auto &[key, val] : colour_hash) {
      layer_to_colours[colour_to_layer[val]].insert(val);
    }

    return remap;
  }

  std::vector<graph::Graph> Features::convert_to_graphs(const data::Dataset dataset) {
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

  void Features::collect_from_dataset(const data::Dataset dataset) {
    if (graph_generator == nullptr) {
      std::string err_msg = "No graph generator is set. Use graph input instead of dataset.";
      throw std::runtime_error(err_msg);
    }
    std::vector<graph::Graph> graphs = convert_to_graphs(dataset);
    collect(graphs);
  }

  void Features::collect(const std::vector<graph::Graph> &graphs) {
    if (pruning == PruningOptions::COLLAPSE_LAYER && collapse_pruned) {
      std::cout << "collect with collapse pruning can only be called at most once" << std::endl;
      exit(-1);
    }

    collecting = true;

    collect_impl(graphs);

    if (pruning == PruningOptions::COLLAPSE_LAYER) {
      collapse_pruned = true;
    }
    collected = true;
    collecting = false;

    // check features have been collected
    if (get_n_features() == 0) {
      throw std::runtime_error("No features have been collected.");
    }
  }

  void Features::layer_redundancy_check() {
    for (int itr = 1; itr < iterations + 1; itr++) {
      if (layer_to_colours[itr].size() == 0) {
        int lower_iterations = itr - 1;
        std::cout << "pruning reduced iterations from " << iterations << " to " << lower_iterations
                  << std::endl;
        iterations = lower_iterations;
        break;
      }
    }
  }

  // overloaded embedding functions
  std::vector<Embedding> Features::embed_dataset(const data::Dataset &dataset) {
    std::vector<graph::Graph> graphs = convert_to_graphs(dataset);
    if (graphs.size() == 0) {
      throw std::runtime_error("No graphs to embed");
    }
    return embed_graphs(graphs);
  }

  std::vector<Embedding> Features::embed_graphs(const std::vector<graph::Graph> &graphs) {
    std::vector<Embedding> X;
    for (const auto &graph : graphs) {
      X.push_back(embed_graph(graph));
    }
    return X;
  }

  Embedding Features::embed_graph(const graph::Graph &graph) {
    return embed(std::make_shared<graph::Graph>(graph));
  }

  Embedding Features::embed_state(const planning::State &state) {
    return embed(graph_generator->to_graph(state));
  }

  /* Prediction functions */

  double Features::predict(const std::shared_ptr<graph::Graph> &graph) {
    if (!store_weights) {
      throw std::runtime_error("Weights have not been set for prediction.");
    }

    Embedding x = embed(graph);
    double h = std::inner_product(x.begin(), x.end(), weights.begin(), 0.0);
    return h;
  }

  double Features::predict(const graph::Graph &graph) {
    return predict(std::make_shared<graph::Graph>(graph));
  }

  double Features::predict(const planning::State &state) {
    std::shared_ptr<graph::Graph> graph = graph_generator->to_graph_opt(state);
    double h = predict(graph);
    graph_generator->reset_graph();
    return h;
  }

  /* Util functions */

  std::string Features::get_string_representation(const Embedding &embedding) {
    std::string str_embed = "";
    for (size_t i = 0; i < embedding.size(); i++) {
      int count = embedding[i];
      if (count == 0) {
        continue;
      }
      str_embed += std::to_string(i) + "." + std::to_string(count) + ".";
    }
    return str_embed;
  }

  std::string Features::get_string_representation(const planning::State &state) {
    Embedding x = embed_state(state);
    return get_string_representation(x);
  }

  // hash type conversion functions
  ColourHash
  Features::str_to_int_colour_hash(std::unordered_map<std::string, int> str_colour_hash) const {
    ColourHash int_colour_hash;
    for (const auto &pair : str_colour_hash) {
      std::vector<int> colour;
      std::istringstream iss(pair.first);
      std::string token;
      while (std::getline(iss, token, '.')) {
        colour.push_back(std::stoi(token));
      }
      int_colour_hash[colour] = pair.second;
    }
    return int_colour_hash;
  }

  std::unordered_map<std::string, int>
  Features::int_to_str_colour_hash(ColourHash int_colour_hash) const {
    std::unordered_map<std::string, int> str_colour_hash;
    for (const auto &pair : int_colour_hash) {
      std::string colour_str = "";
      for (size_t i = 0; i < pair.first.size(); i++) {
        colour_str += std::to_string(pair.first[i]);
        if (i < pair.first.size() - 1) {
          colour_str += ".";
        }
      }
      str_colour_hash[colour_str] = pair.second;
    }
    return str_colour_hash;
  }

  std::vector<long> Features::get_layer_to_n_colours() const {
    std::vector<long> layer_to_n_colours;
    for (size_t i = 0; i < layer_to_colours.size(); i++) {
      layer_to_n_colours.push_back(layer_to_colours[i].size());
    }
    return layer_to_n_colours;
  }

  void Features::set_weights(const std::vector<double> &weights) {
    if (((int)weights.size()) != get_n_features()) {
      throw std::runtime_error("Number of weights must match number of features.");
    }
    store_weights = true;
    this->weights = weights;
  }

  std::vector<double> Features::get_weights() const {
    if (!store_weights) {
      throw std::runtime_error("Cannot get weights as they are not stored.");
    }
    return weights;
  }

  void Features::print_init_colours() const { graph_generator->print_init_colours(); }

  void Features::save(const std::string &filename) {
    // let Python handle file exceptions
    json j;
    j["package_version"] = package_version;
    j["feature_name"] = feature_name;
    j["graph_representation"] = graph_representation;
    j["iterations"] = iterations;
    j["pruning"] = pruning;
    j["multiset_hash"] = multiset_hash;

    j["domain"] = domain->to_json();

    j["colour_hash"] = int_to_str_colour_hash(colour_hash);
    j["colour_to_layer"] = colour_to_layer;

    j["weights"] = weights;

    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
  }
}  // namespace feature_generation
