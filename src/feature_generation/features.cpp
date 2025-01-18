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

    collected = false;
    collecting = false;
    pruned = false;
    store_weights = false;

    colour_hash = new_colour_hash();
    layer_to_colours = new_layer_to_colours();

    initialise_variables();
  }

  void Features::initialise_variables() {
    graph_generator = graph::create_graph_generator(graph_representation, *domain);
    seen_colour_statistics =
        std::vector<std::vector<long>>(2, std::vector<long>(iterations + 1, 0));
    init_neighbour_container();
  }

  std::vector<std::set<int>> Features::new_layer_to_colours() const {
    // plus 1 because zeroth iteration is also included
    return std::vector<std::set<int>>(iterations + 1, std::set<int>());
  }

  VecColourHash Features::new_colour_hash() const {
    VecColourHash ret;
    for (int i = 0; i < iterations + 1; i++) {
      ret.push_back(std::unordered_map<std::vector<int>, int, int_vector_hasher>());
    }
    return ret;
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
    StrColourHash colour_hash_str = j.at("colour_hash").get<StrColourHash>();
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

    // initialise other variables (assume collection already done)
    collected = true;
    collecting = false;
    pruned = true;

    initialise_variables();
  }

  void Features::set_problem(const planning::Problem &problem) {
    if (graph_generator != nullptr) {
      graph_generator->set_problem(problem);
    }
  }

  /* Feature generation functions */

  int Features::get_colour_hash(const std::vector<int> &colour, const int iteration) {
    if (colour.size() == 0) {
      return UNSEEN_COLOUR;
    } else if (!collecting && !colour_hash[iteration].count(colour)) {
#ifdef DEBUGMODE
      std::cout << "UNSEEN ";
      debug_vec(colour);
#endif
      return UNSEEN_COLOUR;
    } else if (collecting && !colour_hash[iteration].count(colour)) {
      int hash = get_n_features();
      colour_hash[iteration][colour] = hash;
      colour_to_layer[hash] = iteration;
      layer_to_colours[iteration].insert(hash);
    }
    return colour_hash[iteration][colour];
  }

  std::map<int, int> Features::remap_colour_hash(const std::set<int> &to_prune) {
    // remap values
    std::map<int, int> remap;
    std::vector<std::vector<std::pair<std::vector<int>, int>>> new_hash_vec(
        iterations + 1, std::vector<std::pair<std::vector<int>, int>>());
    std::unordered_map<int, int> new_colour_layer;

    // layer 0 colours are the same
    for (const auto &[key, val] : colour_hash[0]) {
      new_hash_vec[0].push_back(std::make_pair(key, val));
      new_colour_layer[val] = colour_to_layer[val];
      remap[val] = val;
    }

    // deal with layer 1+ colours
    for (int itr = 1; itr < iterations + 1; itr++) {
      for (const auto &[key, val] : colour_hash.at(itr)) {
        if (to_prune.count(val) > 0) {
          continue;
        }

        // new value is size of new hash + number of layer 0 colours
        int new_val = 0;
        for (size_t i = 0; i < new_hash_vec.size(); i++) {
          new_val += new_hash_vec[i].size();
        }

        remap[val] = new_val;
        new_hash_vec[itr].push_back(std::make_pair(key, new_val));
        new_colour_layer[new_val] = colour_to_layer[val];
      }
    }

    //////////////////////////////////////////
#ifdef DEBUGMODE
    std::cout << "old_hash" << std::endl;
    for (int itr = 1; itr < iterations + 1; itr++) {
      for (const auto &[key, val] : colour_hash[itr]) {
        std::cout << "HASH_ITR " << itr << " HASH ";
        debug_hash(key, val);
      }
    }
    std::cout << "to_prune" << std::endl;
    for (const int i : to_prune) {
      std::cout << "PRUNE " << i << std::endl;
    }
    std::cout << "remap" << std::endl;
    for (const auto &[key, val] : remap) {
      std::cout << "REMAP " << key << " -> " << val << " LAYER: " << new_colour_layer[val]
                << std::endl;
    }
#endif
    //////////////////////////////////////////

    // remap keys
    VecColourHash new_hash(iterations + 1,
                           std::unordered_map<std::vector<int>, int, int_vector_hasher>());
    new_hash[0] = colour_hash[0];
    for (int itr = 1; itr < iterations + 1; itr++) {
      for (size_t i = 0; i < new_hash_vec[itr].size(); i++) {
        std::vector<int> key = new_hash_vec[itr][i].first;
        int val = new_hash_vec[itr][i].second;
        if (new_colour_layer[val] > 0) {
          key = neighbour_container->remap(key, remap);
        }
        new_hash[itr][key] = val;
      }
    }

    // remap hash
    colour_hash = new_hash;

    // remap colours
    colour_to_layer = new_colour_layer;
    layer_to_colours = new_layer_to_colours();
    for (int itr = 0; itr < iterations + 1; itr++) {
      for (const auto &[key, val] : colour_hash[itr]) {
        if (colour_to_layer[val] != itr) {
          std::cout << "error: colour layers not preserved during remap" << std::endl;
          exit(-1);
        }
        layer_to_colours[itr].insert(val);
      }
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
    if (pruning != PruningOptions::NONE && pruned) {
      std::cout << "collect with pruning can only be called at most once" << std::endl;
      exit(-1);
    }

    collecting = true;

    collect_impl(graphs);

    collected = true;
    collecting = false;
    pruned = true;

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

  void Features::add_colour_to_x(int col, int itr, std::vector<int> &x) {
    bool is_seen_colour = (col != UNSEEN_COLOUR);  // prevent branch prediction
    seen_colour_statistics[is_seen_colour][itr]++;
    if (is_seen_colour) {
      x[col]++;
    }
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
  VecColourHash Features::str_to_int_colour_hash(StrColourHash str_colour_hash) const {
    VecColourHash int_colour_hash = new_colour_hash();
    for (int itr = 0; itr < iterations + 1; itr++) {
      for (const auto &pair : str_colour_hash[itr]) {
        std::vector<int> colour;
        std::istringstream iss(pair.first);
        std::string token;
        while (std::getline(iss, token, '.')) {
          colour.push_back(std::stoi(token));
        }
        int_colour_hash[itr][colour] = pair.second;
      }
    }
    return int_colour_hash;
  }

  StrColourHash Features::int_to_str_colour_hash(VecColourHash int_colour_hash) const {
    StrColourHash str_colour_hash;
    for (int itr = 0; itr < iterations + 1; itr++) {
      str_colour_hash.push_back(std::unordered_map<std::string, int>());
      for (const auto &pair : int_colour_hash[itr]) {
        std::string colour_str = "";
        for (size_t i = 0; i < pair.first.size(); i++) {
          colour_str += std::to_string(pair.first[i]);
          if (i < pair.first.size() - 1) {
            colour_str += ".";
          }
        }
        str_colour_hash[itr][colour_str] = pair.second;
      }
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

  int Features::get_n_features() const {
    int ret = 0;
    for (int i = 0; i < iterations + 1; i++) {
      ret += colour_hash[i].size();
    }
    return ret;
  }

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
