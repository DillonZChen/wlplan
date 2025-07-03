#include "../../../include/feature_generator/feature_generators/ccwla.hpp"

#include "../../../include/graph_generator/graph_generator_factory.hpp"
#include "../../../include/utils/nlohmann/json.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace feature_generator {
  CCWLaFeatures::CCWLaFeatures(const planning::Domain &domain,
                               std::string graph_representation,
                               int iterations,
                               std::string pruning,
                               bool multiset_hash)
      : CCWLFeatures("ccwl-a", domain, graph_representation, iterations, pruning, multiset_hash) {}

  CCWLaFeatures::CCWLaFeatures(const std::string &filename) : CCWLFeatures(filename) {}

  CCWLaFeatures::CCWLaFeatures(const std::string &filename, bool quiet)
      : CCWLFeatures(filename, quiet) {}

  int CCWLaFeatures::get_n_features() const {
    int n_cat_features = get_n_colours();
    int n_con_features = get_n_colours();
    int n_sub_features = n_con_features * (n_con_features - 1);
    return n_cat_features + n_con_features + n_sub_features;
  }

  Embedding CCWLaFeatures::embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) {
    Embedding ccwl_embedding = CCWLFeatures::embed_impl(graph);
    int n_con_features = get_n_colours();  // = n_cat_features
    for (int i = 0; i < n_con_features; i++) {
      for (int j = 0; j < n_con_features; j++) {
        if (i == j)
          continue;
        ccwl_embedding.push_back(std::max(ccwl_embedding[i] - ccwl_embedding[j], 0.0));
      }
    }

    return ccwl_embedding;
  }
}  // namespace feature_generator
