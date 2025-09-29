#ifndef FEATURE_GENERATOR_FEATURES_HPP
#define FEATURE_GENERATOR_FEATURES_HPP

#include "../data/dataset.hpp"
#include "../graph_generator/graph.hpp"
#include "../graph_generator/graph_generator.hpp"
#include "../planning/domain.hpp"
#include "../planning/state.hpp"
#include "neighbour_container.hpp"
#include "pruning_options.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#define UNSEEN_COLOUR -1

#define debug_hash(k, v)                                                                           \
  for (const int i : k) {                                                                          \
    std::cout << i << ".";                                                                         \
  }                                                                                                \
  std::cout << " : " << v << std::endl;
#define debug_vec(k)                                                                               \
  for (const int i : k) {                                                                          \
    std::cout << i << ".";                                                                         \
  }                                                                                                \
  std::cout << std::endl;

class int_vector_hasher {
 public:
  // https://stackoverflow.com/a/27216842
  std::size_t operator()(std::vector<int> const &vec) const {
    std::size_t seed = vec.size();
    for (auto &i : vec) {
      seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

namespace wlplan {
  namespace feature_generator {
    using Embedding = std::vector<double>;
    using ColourHash = std::unordered_map<std::vector<int>, int, int_vector_hasher>;
    using VecColourHash = std::vector<ColourHash>;
    using StrColourHash = std::vector<std::unordered_map<std::string, int>>;

    class Features {
     protected:
      // configurations [saved]
      std::string package_version;
      std::string feature_name;
      std::string graph_representation;
      int iterations;  // equivalently, layers
      std::string pruning;
      bool multiset_hash;

      // colouring [saved]
      VecColourHash colour_hash;
      std::unordered_map<int, int> colour_to_layer;
      std::vector<std::set<int>> layer_to_colours;

      // optional linear weights [saved]
      bool store_weights;
      std::vector<double> weights;

      // helper variables
      std::shared_ptr<planning::Domain> domain;
      std::shared_ptr<graph_generator::GraphGenerator> graph_generator;
      std::shared_ptr<NeighbourContainer> neighbour_container;
      bool collected;
      bool collecting;
      bool pruned;

      // logger variables
      bool quiet;

      // runtime statistics; int is faster than long but could cause overflow
      // [i][j] denotes seen count if i=1, and unseen count if i=0
      // for iteration j = 0, ..., iterations - 1
      std::vector<std::vector<long>> seen_colour_statistics;

      // get hashed colour if it exists, and constructs it if it doesn't
      int get_colour_hash(const std::vector<int> &colour, const int iteration);
      // fast ver. that assumes no unseen colours (e.g. collecting), and does not store itr info
      int get_colour_hash_fast(const std::vector<int> &colour, const int iteration);

      // reformat colour hash based on colours to throw out
      VecColourHash new_colour_hash() const;
      std::vector<std::set<int>> new_layer_to_colours() const;
      std::map<int, int> remap_colour_hash(std::set<int> &to_prune);

      // check if configuration is valid
      void check_valid_configuration();

      // common init for initialisation and loading from file
      void initialise_variables();

      // main virtual functions
      virtual void collect_impl(const std::vector<graph_generator::Graph> &graphs) = 0;
      virtual Embedding embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) = 0;

     public:
      Features(const std::string feature_name,
               const planning::Domain &domain,
               std::string graph_representation,
               int iterations,
               std::string pruning,
               bool multiset_hash);

      Features(const std::string &filename);

      Features(const std::string &filename, const bool quiet);

      virtual ~Features() = default;

      /* Feature generation functions */

      // convert states to graphs
      std::vector<graph_generator::Graph> to_graphs(const data::DomainDataset dataset);

      // collect training colours
      void collect_from_dataset(const data::DomainDataset dataset);
      void collect(const std::vector<graph_generator::Graph> &graphs);
      // for novelty heuristics
      std::unordered_map<int, int> virtual collect_embed(const planning::State &state);
      void layer_redundancy_check();

      // embedding assumes training is done, and returns a feature matrix X
      std::vector<Embedding> embed_dataset(const data::DomainDataset &dataset);
      std::vector<Embedding> embed_graphs(const std::vector<graph_generator::Graph> &graphs);
      Embedding embed_graph(const graph_generator::Graph &graph);
      Embedding embed_state(const planning::State &state);
      Embedding embed(const std::shared_ptr<graph_generator::Graph> &graph);

      void add_colour_to_x(int colour, int iteration, Embedding &x);

      /* Pruning functions */

      // output maps equivalent features to the same group
      std::map<int, int> get_equivalence_groups(const std::vector<Embedding> &X);
      void prune_this_iteration(int iteration,
                                const std::vector<graph_generator::Graph> &graphs,
                                std::vector<std::vector<int>> &cur_colours);
      void prune_bulk(const std::vector<graph_generator::Graph> &graphs);

      std::set<int> prune_collapse_layer(int iteration, std::vector<std::vector<int>> &cur_colours);
      std::set<int> prune_collapse_layer_greedy(int iteration,
                                                const std::vector<graph_generator::Graph> &graphs);
      std::set<int> prune_collapse_layer_maxsat(int iteration,
                                                const std::vector<graph_generator::Graph> &graphs);
      std::set<int>
      prune_collapse_layer_frequency(int iteration,
                                     const std::vector<graph_generator::Graph> &graphs);
      std::set<int> prune_maxsat(std::vector<Embedding> X);
      std::set<int> prune_maxsat(std::vector<Embedding> X, const int maxsat_iterations);

      /* Prediction functions */

      double predict(const std::shared_ptr<graph_generator::Graph> &graph);
      double predict(const graph_generator::Graph &graph);
      double predict(const planning::State &state);

      void set_weights(const std::vector<double> &weights);
      std::vector<double> get_weights() const;

      /* Getter functions */

      std::string get_feature_name() const { return feature_name; }
      std::shared_ptr<planning::Domain> get_domain() const { return domain; }
      std::string get_graph_representation() const { return graph_representation; }
      int get_iterations() const { return iterations; }
      std::string get_pruning() { return pruning; }
      void set_pruning(const std::string &pruning) { this->pruning = pruning; }
      std::set<int> get_iteration_colours(int iteration) const {
        return layer_to_colours.at(iteration);
      }
      VecColourHash get_colour_hash() { return colour_hash; }

      /* Util functions */

      void log_iteration(int iteration) const;
      void be_quiet();

      // get string representation of WL colours agnostic to the number of collected colours
      std::string get_string_representation(const Embedding &embedding);
      std::string get_string_representation(const planning::State &state);

      // set problem for graph generator if it exists
      void set_problem(const planning::Problem &problem);

      // conversion between vectors and strings
      VecColourHash str_to_int_colour_hash(StrColourHash str_colour_hash) const;
      StrColourHash int_to_str_colour_hash(VecColourHash int_colour_hash) const;

      // statistics functions
      int get_n_colours() const;
      virtual int get_n_features() const = 0;
      std::vector<long> get_seen_counts() const { return seen_colour_statistics[1]; };
      std::vector<long> get_unseen_counts() const { return seen_colour_statistics[0]; };
      std::vector<long> get_layer_to_n_colours() const;
      void print_init_colours() const;

      std::vector<std::set<int>> get_layer_to_colours() const;
      std::unordered_map<int, int> get_colour_to_layer() const { return colour_to_layer; };

      void save(const std::string &filename);
      void save(const std::string &filename, const std::vector<double> &weights);
    };
  }  // namespace feature_generator
}  // namespace wlplan

#endif  // FEATURE_GENERATOR_FEATURES_HPP
