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

  Embedding CCWLaFeatures::embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) {
    Embedding ccwl_embedding = CCWLFeatures::embed_impl(graph);
    int n_con_features = get_n_features();  // = n_cat_features
    for (int i = 0; i < n_con_features; i++) {
      for (int j = 0; j < n_con_features; j++) {
        if (i == j)
          continue;
        ccwl_embedding.push_back(std::max(ccwl_embedding[i] - ccwl_embedding[j], 0.0));
      }
    }

    return ccwl_embedding;
  }

  void CCWLaFeatures::set_weights(const std::vector<double> &weights) {
    int n_cat_features = get_n_features();
    int n_con_features = get_n_features();
    int n_sub_features = n_con_features * (n_con_features - 1);

    int n_expected_features = n_cat_features + n_con_features + n_sub_features;
    int n_weights = ((int)weights.size());

    if (n_weights != n_expected_features) {
      throw std::runtime_error("Number of weights " + std::to_string(n_weights) +
                               " does not match the number of features " +
                               std::to_string(n_expected_features) + ".");
    }
    store_weights = true;
    this->weights = weights;
  }
}  // namespace feature_generator
