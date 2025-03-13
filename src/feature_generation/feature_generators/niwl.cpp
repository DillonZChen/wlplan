#include "../../../include/feature_generation/feature_generators/niwl.hpp"

#include "../../../include/graph/graph_generator_factory.hpp"
#include "../../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace feature_generation {
  NIWLFeatures::NIWLFeatures(const planning::Domain &domain,
                             std::string graph_representation,
                             int iterations,
                             std::string pruning,
                             bool multiset_hash)
      : IWLFeatures("niwl", domain, graph_representation, iterations, pruning, multiset_hash) {}

  NIWLFeatures::NIWLFeatures(const std::string &filename) : IWLFeatures(filename) {}

  NIWLFeatures::NIWLFeatures(const std::string &filename, bool quiet)
      : IWLFeatures(filename, quiet) {}

  Embedding NIWLFeatures::embed_impl(const std::shared_ptr<graph::Graph> &graph) {
    Embedding iwl_embedding = IWLFeatures::embed_impl(graph);
    double n = (double)graph->get_n_nodes();
    for (size_t i = 0; i < iwl_embedding.size(); i++) {
      iwl_embedding[i] = iwl_embedding[i] / n;
    }
    return iwl_embedding;
  }
}  // namespace feature_generation
