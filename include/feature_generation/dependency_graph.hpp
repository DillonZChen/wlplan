#ifndef FEATURE_GENERATION_DEPENDENCY_GRAPH_HPP
#define FEATURE_GENERATION_DEPENDENCY_GRAPH_HPP

#include "features.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace feature_generation {
  class FeatureDependencyGraph {
   protected:
    std::vector<std::vector<int>> edges_fw;
    std::vector<std::vector<int>> edges_bw;

   public:
    FeatureDependencyGraph(ColourHash colour_hash);

    const std::vector<std::vector<int>> &get_fw_edges() { return edges_fw; };
    const std::vector<int> &get_fw_edges(int node) { return edges_fw[node]; };
    const std::vector<std::vector<int>> &get_bw_edges() { return edges_bw; };
    const std::vector<int> &get_bw_edges(int node) { return edges_bw[node]; };

    int num_nodes() { return edges_fw.size(); };
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_DEPENDENCY_GRAPH_HPP
