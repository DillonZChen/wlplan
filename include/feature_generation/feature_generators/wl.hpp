#ifndef FEATURE_GENERATION_FEATURE_GENERATORS_WL_HPP
#define FEATURE_GENERATION_FEATURE_GENERATORS_WL_HPP

#include "../features.hpp"

#include <memory>
#include <string>
#include <vector>

namespace feature_generation {
  class WLFeatures : public Features {
   public:
    WLFeatures(const std::string wl_name,
               const planning::Domain &domain,
               std::string graph_representation,
               int iterations,
               std::string pruning,
               bool multiset_hash);

    WLFeatures(const planning::Domain &domain,
               std::string graph_representation,
               int iterations,
               std::string pruning,
               bool multiset_hash);

    WLFeatures(const std::string &filename);

    Embedding embed_impl(const std::shared_ptr<graph::Graph> &graph) override;

   protected:
    void collect_impl(const std::vector<graph::Graph> &graphs) override;
    void refine(const std::shared_ptr<graph::Graph> &graph,
                std::set<int> &nodes,
                std::vector<int> &colours,
                int iteration);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_FEATURE_GENERATORS_WL_HPP
