#ifndef FEATURE_GENERATION_WL_FEATURES_HPP
#define FEATURE_GENERATION_WL_FEATURES_HPP

#include "../data/dataset.hpp"
#include "../graph/graph.hpp"
#include "../graph/graph_generator.hpp"
#include "../planning/domain.hpp"
#include "../planning/state.hpp"
#include "neighbour_container.hpp"

#include <memory>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <string>
#include <vector>

namespace feature_generation {
  using Embedding = std::vector<int>;

  class WLFeatures {
   public:
    WLFeatures(const planning::Domain &domain,
               std::string graph_representation,
               int iterations,
               std::string prune_features,
               bool multiset_hash);

    // collect training colours
    void collect(const data::Dataset dataset);

    void collect(const std::vector<graph::Graph> &graphs);

    // set problem for graph generator if it exists
    void set_problem(const planning::Problem &problem);

    // assumes training is done, and returns a numpy feature matrix X
    py::array_t<int> embed_np(const data::Dataset dataset);

    py::array_t<int> embed_np(const std::vector<graph::Graph> &graphs);

    py::array_t<int> embed_np(const planning::State &state);

    std::vector<Embedding> embed(const std::vector<graph::Graph> &graphs);

    Embedding embed(const graph::Graph &graph);

    Embedding embed(const planning::State &state);

    /* statistics functions */
    int get_n_features() const { return colours_to_keep.size(); }

    std::vector<long> get_seen_counts() const { return seen_colour_statistics[1]; };

    std::vector<long> get_unseen_counts() const { return seen_colour_statistics[0]; };

   private:
    std::shared_ptr<graph::GraphGenerator> graph_generator;

    // state of the class
    bool collected;
    bool collecting;
    int cur_collecting_layer;
    std::shared_ptr<NeighbourContainer> neighbour_container;

    // configurations
    int iterations;  // equivalently, layers
    std::string prune_features;
    bool multiset_hash;

    // colouring
    std::unordered_map<std::string, int> colour_hash;
    std::vector<int> colour_to_layer;
    std::vector<int> colours_to_keep;

    // runtime statistics; int is faster than long but could cause overflow
    // [i][j] denotes seen count if i=1, and unseen count if i=0
    // for iteration j = 0, ..., iterations - 1
    std::vector<std::vector<long>> seen_colour_statistics;

    // convert to ILG
    std::vector<graph::Graph> convert_to_graphs(const data::Dataset dataset);

    // get hashed colour if it exists, and constructs it if it doesn't
    int get_colour_hash(const std::string &colour);

    // main WL iteration, updates colours and uses colours_tmp as extra memory for refining
    void
    refine(const graph::Graph &graph, std::vector<int> &colours, std::vector<int> &colours_tmp);
  };

}  // namespace feature_generation

#endif  // FEATURE_GENERATION_WL_FEATURES_HPP
