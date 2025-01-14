#include "../../include/feature_generation/dependency_graph.hpp"

namespace feature_generation {
  FeatureDependencyGraph::FeatureDependencyGraph(ColourHash colour_hash) {
    // initialise graph with nodes
    edges_fw.resize(colour_hash.size());
    edges_bw.resize(colour_hash.size());

    // TODO
  }
}  // namespace feature_generation
