#include "../../include/feature_generator/features.hpp"

#include "../../include/feature_generator/maxsat.hpp"
#include "../../include/feature_generator/neighbour_containers/kwl2_neighbour_container.hpp"
#include "../../include/feature_generator/neighbour_containers/lwl2_neighbour_container.hpp"
#include "../../include/feature_generator/neighbour_containers/wl_neighbour_container.hpp"
#include "../../include/graph_generator/graph_generator_factory.hpp"
#include "../../include/utils/exceptions.hpp"
#include "../../include/utils/nlohmann/json.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using json = nlohmann::json;

namespace feature_generator {
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
    quiet = false;
    check_valid_configuration();

    this->domain = std::make_shared<planning::Domain>(domain);

    collected = false;
    collecting = false;
    pruned = false;
    store_weights = false;

    colour_hash = new_colour_hash();
    layer_to_colours = new_layer_to_colours();

    initialise_variables();
  }

  void Features::check_valid_configuration() {
    // check pruning support
    if (pruning != PruningOptions::NONE &&
        !std::set<std::string>({"wl", "2-lwl"}).count(feature_name)) {
      throw NotSupportedError("Pruning option `" + pruning + "` for feature option `" +
                              feature_name + "`");
    }
  }

  void Features::initialise_variables() {
    graph_generator = graph_generator::init_feature_generator(graph_representation, *domain);
    seen_colour_statistics =
        std::vector<std::vector<long>>(2, std::vector<long>(iterations + 1, 0));

    // We use a factory style method here instead of a virtual function as this is called
    // from a constructor, from which virtual functions are not allowed to be called.
    if (std::set<std::string>({"wl", "ccwl", "ccwl-a", "iwl", "niwl"}).count(feature_name)) {
      neighbour_container = std::make_shared<WLNeighbourContainer>(multiset_hash);
    } else if (feature_name == "2-kwl") {
      neighbour_container = std::make_shared<KWL2NeighbourContainer>(multiset_hash);
    } else if (feature_name == "2-lwl") {
      neighbour_container = std::make_shared<LWL2NeighbourContainer>(multiset_hash);
    } else {
      throw NotImplementedError("Neighbour container for feature_name=" + feature_name);
    }
  }

  std::vector<std::set<int>> Features::new_layer_to_colours() const {
    return std::vector<std::set<int>>(iterations + 1, std::set<int>());
  }

  VecColourHash Features::new_colour_hash() const {
    VecColourHash ret;
    for (int i = 0; i < iterations + 1; i++) {
      ret.push_back(std::unordered_map<std::vector<int>, int, int_vector_hasher>());
    }
    return ret;
  }

  Features::Features(const std::string &filename) : Features(filename, false) {}

  Features::Features(const std::string &filename, const bool quiet) {
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

    std::vector<std::pair<std::string, int>> raw_schemata =
        j.at("domain").at("schemata").get<std::vector<std::pair<std::string, int>>>();
    std::vector<planning::Schema> domain_schemata = std::vector<planning::Schema>();
    for (size_t i = 0; i < raw_schemata.size(); i++) {
      domain_schemata.push_back(planning::Schema(raw_schemata[i].first, raw_schemata[i].second));
    }

    std::vector<planning::Object> constant_objects =
        j.at("domain").at("constant_objects").get<std::vector<planning::Object>>();
    domain = std::make_shared<planning::Domain>(
        domain_name, domain_predicates, domain_functions, domain_schemata, constant_objects);

    // load weights if they exist
    std::vector<double> weights_tmp = j.at("weights").get<std::vector<double>>();
    if (weights_tmp.size() > 0) {
      store_weights = true;
      weights = weights_tmp;
    } else {
      store_weights = false;
    }

    // reconstruct layer to colours
    layer_to_colours = get_layer_to_colours();

    // initialise other variables (assume collection already done)
    collected = true;
    collecting = false;
    pruned = true;

    initialise_variables();

    if (!quiet) {
      std::cout << "package_version=" << package_version << std::endl;
      std::cout << "feature_name=" << feature_name << std::endl;
      std::cout << "graph_representation=" << graph_representation << std::endl;
      std::cout << "iterations=" << iterations << std::endl;
      std::cout << "pruning=" << pruning << std::endl;
      std::cout << "multiset_hash=" << multiset_hash << std::endl;
      std::cout << "domain=" << domain->to_string() << std::endl;
      std::cout << "weights_size=" << weights_tmp.size() << std::endl;
    }
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
      int hash = get_n_colours();
      colour_hash[iteration][colour] = hash;
      colour_to_layer[hash] = iteration;
      layer_to_colours[iteration].insert(hash);
    }
    return colour_hash[iteration][colour];
  }

  std::map<int, int> Features::remap_colour_hash(std::set<int> &to_prune) {

    //////////////////////////////////////////
#ifdef DEBUGMODE
    std::cout << "****************************************************************" << std::endl;
    for (int itr = 1; itr < iterations + 1; itr++) {
      for (const auto &[key, val] : colour_hash[itr]) {
        std::cout << "HASH_ITR " << itr << " HASH ";
        debug_hash(key, val);
      }
    }
    std::cout << "PRUNE";
    for (const int i : to_prune) {
      std::cout << " " << i;
    }
    std::cout << std::endl;
#endif
    //////////////////////////////////////////

    // remap values
    std::map<int, int> remap;
    std::vector<std::vector<std::pair<std::vector<int>, int>>> new_hash_vec(
        iterations + 1, std::vector<std::pair<std::vector<int>, int>>());
    std::unordered_map<int, int> new_colour_layer;

    int modifications = 1;
    int post_pruning_itr = 0;
    while (modifications > 0) {
      std::cout << "Post-pruning iteration " << post_pruning_itr << std::endl;
      post_pruning_itr++;
      modifications = 0;
      for (int itr = 1; itr < iterations + 1; itr++) {
        for (const auto &[key, val] : colour_hash.at(itr)) {
          // if to_prune appears in any value or key in colour hash, discard it
          if (to_prune.count(val) > 0) {
            continue;
          }

          bool skip = false;
          for (const int &neighbour : neighbour_container->get_neighbour_colours(key)) {
            if (to_prune.count(neighbour) > 0) {
              skip = true;
              break;
            }
          }
          if (skip) {
            to_prune.insert(val);
            modifications++;
          }
        }
      }
      std::cout << "Pruned an additional " << modifications << " features" << std::endl;
    }

    for (int itr = 0; itr < iterations + 1; itr++) {
      for (const auto &[key, val] : colour_hash.at(itr)) {
        // if to_prune appears in any value or key in colour hash, discard it
        if (to_prune.count(val) > 0) {
          continue;
        }

        // otherwise remap
        int new_val = remap.size();
        remap[val] = new_val;
        new_hash_vec[itr].push_back(std::make_pair(key, new_val));
        new_colour_layer[new_val] = colour_to_layer[val];
      }
    }

    //////////////////////////////////////////
#ifdef DEBUGMODE
    for (const auto &[key, val] : remap) {
      std::cout << "REMAP " << key << " -> " << val << " LAYER: " << new_colour_layer[val]
                << std::endl;
    }
#endif
    //////////////////////////////////////////

    // remap keys
    VecColourHash new_hash(iterations + 1,
                           std::unordered_map<std::vector<int>, int, int_vector_hasher>());

    // layer 0 keeps same keys because they are from graph init node colours
    for (size_t i = 0; i < new_hash_vec[0].size(); i++) {
      std::vector<int> key = new_hash_vec[0][i].first;
      int val = new_hash_vec[0][i].second;
      new_hash[0][key] = val;
    }

    // other layers (>=1) remap keys
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
    colour_to_layer = new_colour_layer;
    layer_to_colours = new_layer_to_colours();
    for (int itr = 0; itr < iterations + 1; itr++) {
      for (const auto &[key, val] : colour_hash[itr]) {
#ifdef DEBUGMODE
        if (colour_to_layer[val] != itr) {
          std::cout << "ERROR: colour layers not preserved during remap. " << std::endl;
          std::cout << "old layer: " << itr << std::endl;
          std::cout << "new layer: " << colour_to_layer[val] << std::endl;
          debug_hash(key, val);
          std::cout << "Exiting." << std::endl;
          exit(-1);
        }
#endif
        layer_to_colours[itr].insert(val);
      }
    }

    //////////////////////////////////////////
#ifdef DEBUGMODE
    std::cout << "--- NEW COLOUR_HASH AFTER REMAP ---" << std::endl;
    for (int itr = 0; itr < iterations + 1; itr++) {
      for (const auto &[key, val] : colour_hash[itr]) {
        std::cout << "HASH_ITR " << itr << " HASH ";
        debug_hash(key, val);
      }
    }
    std::cout << "--- NEW COLOUR_TO_LAYER AFTER REMAP ---" << std::endl;
    for (const auto &[key, val] : colour_to_layer) {
      std::cout << "COL " << key << " LAYER " << val << std::endl;
    }
    std::cout << "--- NEW LAYER_TO_COLOURS AFTER REMAP ---" << std::endl;
    for (int itr = 0; itr < iterations + 1; itr++) {
      std::cout << "LAYER " << itr << " COLS ";
      for (const int col : layer_to_colours[itr]) {
        std::cout << col << " ";
      }
      std::cout << std::endl;
    }
    std::cout << "****************************************************************" << std::endl;
#endif
    //////////////////////////////////////////

    return remap;
  }

  std::vector<graph_generator::Graph> Features::to_graphs(const data::DomainDataset dataset) {
    return graph_generator->to_graphs(dataset);
  }

  void Features::collect_from_dataset(const data::DomainDataset dataset) {
    if (graph_generator == nullptr) {
      throw std::runtime_error("No graph generator is set. Use graph input instead of dataset.");
    }
    std::vector<graph_generator::Graph> graphs = to_graphs(dataset);
    collect(graphs);
  }

  void Features::collect(const std::vector<graph_generator::Graph> &graphs) {
    if (pruning != PruningOptions::NONE && pruned) {
      throw std::runtime_error("Collect with pruning can only be called at most once");
    }

    collecting = true;

    collect_impl(graphs);

    std::cout << "[complete]" << std::endl;

    // bulk pruning
    prune_bulk(graphs);
    layer_redundancy_check();

    collected = true;
    collecting = false;

    // check features have been collected
    if (get_n_colours() == 0) {
      std::cout << "WARNING: no features have been collected" << std::endl;
    }
  }

  std::unordered_map<int, int> Features::collect_embed(const planning::State &state) {
    (void)state;  // unused in this implementation
    throw NotImplementedError("collect_embed() is not implemented for this feature generator. "
                              "Use collect() and embed() instead.");
  }

  void Features::layer_redundancy_check() {
    for (int itr = 1; itr < iterations + 1; itr++) {
      if (layer_to_colours[itr].size() == 0) {
        int lower_iterations = itr - 1;
        std::cout << "Pruning reduced iterations from " << iterations << " to " << lower_iterations
                  << std::endl;
        iterations = lower_iterations;
        break;
      }
    }
  }

  // overloaded embedding functions
  std::vector<Embedding> Features::embed_dataset(const data::DomainDataset &dataset) {
    std::vector<graph_generator::Graph> graphs = to_graphs(dataset);
    if (graphs.size() == 0) {
      throw std::runtime_error("No graphs to embed");
    }
    return embed_graphs(graphs);
  }

  std::vector<Embedding> Features::embed_graphs(const std::vector<graph_generator::Graph> &graphs) {
    std::vector<Embedding> X;
    for (const auto &graph : graphs) {
      X.push_back(embed_graph(graph));
    }
    return X;
  }

  Embedding Features::embed_graph(const graph_generator::Graph &graph) {
    return embed_impl(std::make_shared<graph_generator::Graph>(graph));
  }

  Embedding Features::embed_state(const planning::State &state) {
    return embed_impl(graph_generator->to_graph(state));
  }

  Embedding Features::embed(const std::shared_ptr<graph_generator::Graph> &graph) {
    collecting = false;
    if (!collected) {
      throw std::runtime_error("collect() must be called before embedding");
    }

    return embed_impl(graph);
  }

  void Features::add_colour_to_x(int col, int itr, Embedding &x) {
    bool is_seen_colour = (col != UNSEEN_COLOUR);  // prevent branch prediction
    seen_colour_statistics[is_seen_colour][itr]++;
    if (is_seen_colour) {
      x[col]++;
    }
  }

  /* Pruning functions (see pruning/ source files for specific implementations) */

  std::map<int, int> Features::get_equivalence_groups(const std::vector<Embedding> &X) {
    std::map<int, int> feature_group;
    int n_features = X[0].size();
    std::unordered_map<std::vector<int>, int, int_vector_hasher> canonical_group;
    for (int colour = 0; colour < n_features; colour++) {
      std::vector<int> feature;
      for (size_t j = 0; j < X.size(); j++) {
        feature.push_back(X[j][colour]);
      }

      int group;
      if (canonical_group.count(feature) == 0) {  // new feature
        group = canonical_group.size();
        canonical_group[feature] = group;
      } else {  // seen this feature before
        group = canonical_group.at(feature);
      }

      feature_group[colour] = group;
    }

    return feature_group;
  }

  /* Prediction functions */

  double Features::predict(const std::shared_ptr<graph_generator::Graph> &graph) {
    if (!store_weights) {
      throw std::runtime_error("Weights have not been set for prediction.");
    }

    Embedding x = embed_impl(graph);
    double h = std::inner_product(x.begin(), x.end(), weights.begin(), 0.0);
    return h;
  }

  double Features::predict(const graph_generator::Graph &graph) {
    return predict(std::make_shared<graph_generator::Graph>(graph));
  }

  double Features::predict(const planning::State &state) {
    std::shared_ptr<graph_generator::Graph> graph = graph_generator->to_graph_opt(state);
    double h = predict(graph);
    graph_generator->reset_graph();
    return h;
  }

  /* Util functions */
  void Features::log_iteration(int iteration) const {
    if (!quiet)
      std::cout << "[Iteration " << iteration << "]\nCollecting." << std::endl;
  };
  void Features::be_quiet() { quiet = true; }

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

  std::vector<std::set<int>> Features::get_layer_to_colours() const {
    std::vector<std::set<int>> ret(iterations + 1, std::set<int>());
    for (auto &[colour, layer] : colour_to_layer) {
      ret[layer].insert(colour);
    }
#ifndef NDEBUG
    for (int i = 0; i < iterations + 1; i++) {
      if (ret[i].size() != layer_to_colours[i].size()) {
        std::cout << "ERROR: layer_to_colours and colour_to_layer do not match." << std::endl;
        std::cout << "layer_to_colours[" << i << "].size()=" << layer_to_colours[i].size()
                  << std::endl;
        std::cout << "ret[" << i << "].size()=" << ret[i].size() << std::endl;
        exit(-1);
      }
    }
#endif
    return ret;
  }

  void Features::set_weights(const std::vector<double> &weights) {
    if (((int)weights.size()) != get_n_features()) {
      std::string msg = "Number of weights (" + std::to_string(weights.size()) + ") " +
                        "must match number of features (" + std::to_string(get_n_features()) + ").";
      throw std::runtime_error(msg);
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

  int Features::get_n_colours() const {
    int ret = 0;
    for (int i = 0; i < iterations + 1; i++) {
      ret += colour_hash[i].size();
    }
    return ret;
  }

  bool create_directory_recursive(std::string const &dirName, std::error_code &err) {
    // https://stackoverflow.com/a/71658518
    err.clear();
    if (!std::filesystem::create_directories(dirName, err)) {
      if (std::filesystem::exists(dirName)) {
        // The folder already exists:
        err.clear();
        return true;
      }
      return false;
    }
    return true;
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

    // Create directory if it doesn't exist
    if (filename.find_last_of("/") != std::string::npos) {
      std::error_code err;
      std::string directory_name = filename.substr(0, filename.find_last_of("/"));
      if (!create_directory_recursive(directory_name, err)) {
        std::cout << "Error: failed to recursively create directory. " << err.message()
                  << std::endl;
      }
    }

    // Save to file
    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
  }

  void Features::save(const std::string &filename, const std::vector<double> &weights) {
    set_weights(weights);
    save(filename);
  }
}  // namespace feature_generator
