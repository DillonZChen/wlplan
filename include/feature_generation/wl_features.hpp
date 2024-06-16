#ifndef FEATURE_GENERATION_WL_FEATURES_HPP
#define FEATURE_GENERATION_WL_FEATURES_HPP

#include "../data/dataset.hpp"
#include "../graph/graph.hpp"
#include "../graph/graph_generator.hpp"
#include "../planning/domain.hpp"
#include "../planning/state.hpp"
#include "neighbour_container.hpp"

#include <memory>
#include <string>
#include <vector>

namespace feature_generation {
  using Embedding = std::vector<int>;

  class WLFeatures {
   private:
    // configurations [saved]
    std::string package_version;
    std::string graph_representation;
    int iterations;  // equivalently, layers
    std::string prune_features;
    bool multiset_hash;

    // colouring [saved]
    std::unordered_map<std::string, int> colour_hash;
    std::vector<int> colour_to_layer;
    std::vector<int> colours_to_keep;

    // optional linear weights [saved]
    bool store_weights;
    std::vector<double> weights;

    // helper variables
    std::shared_ptr<planning::Domain> domain;
    std::shared_ptr<graph::GraphGenerator> graph_generator;
    bool collected;
    bool collecting;
    int cur_collecting_layer;
    std::shared_ptr<NeighbourContainer> neighbour_container;

    // runtime statistics; int is faster than long but could cause overflow
    // [i][j] denotes seen count if i=1, and unseen count if i=0
    // for iteration j = 0, ..., iterations - 1
    std::vector<std::vector<long>> seen_colour_statistics;

   public:
    WLFeatures(const planning::Domain &domain,
               std::string graph_representation,
               int iterations,
               std::string prune_features,
               bool multiset_hash);

    WLFeatures(const std::string &filename);

    /* feature functions */

    // collect training colours
    void collect(const data::Dataset dataset);

    void collect(const std::vector<graph::Graph> &graphs);

    // set problem for graph generator if it exists
    void set_problem(const planning::Problem &problem);

    // assumes training is done, and returns a feature matrix X
    std::vector<Embedding> embed(const data::Dataset &dataset);

    std::vector<Embedding> embed(const std::vector<graph::Graph> &graphs);

    Embedding embed(const graph::Graph &graph);

    Embedding embed(const planning::State &state);

    /* prediction functions */

    void set_weights(const std::vector<double> &weights);

    std::vector<double> get_weights() const;

    double predict(const graph::Graph &graph);

    double predict(const planning::State &state);

    /* statistics functions */

    int get_n_features() const { return colours_to_keep.size(); }

    std::vector<long> get_seen_counts() const { return seen_colour_statistics[1]; };

    std::vector<long> get_unseen_counts() const { return seen_colour_statistics[0]; };

    /* I/O functions */
    void save(const std::string &filename);

   private:
    // get hashed colour if it exists, and constructs it if it doesn't
    int get_colour_hash(const std::string &colour);

    // main WL iteration, updates colours and uses colours_tmp as extra memory for refining
    void
    refine(const graph::Graph &graph, std::vector<int> &colours, std::vector<int> &colours_tmp);

    // convert to ILG
    std::vector<graph::Graph> convert_to_graphs(const data::Dataset dataset);
  };

}  // namespace feature_generation

#endif  // FEATURE_GENERATION_WL_FEATURES_HPP
