#include "../../include/feature_generation/dependency_graph.hpp"

namespace feature_generation {
  FeatureDependencyGraph::FeatureDependencyGraph(ColourHash colour_hash) {
    // initialise graph with nodes
    edges_fw = std::vector<std::vector<int>>(colour_hash.size(), std::vector<int>());
    edges_bw = std::vector<std::vector<int>>(colour_hash.size(), std::vector<int>());

    // TODO this only works for 1-WL variants
    for (const auto &[neighbours, colour] : colour_hash) {  // std::vector<int>, int
      std::vector<int> ancestors = {neighbours.at(0)};
      for (size_t i = 1; i < neighbours.size(); i+=2) {
        ancestors.push_back(neighbours[i]);
      }
      for (const int ancestor: ancestors) {
        edges_fw[ancestor].push_back(colour);
        edges_bw[colour].push_back(ancestor);
      }
    }
  }
}  // namespace feature_generation
