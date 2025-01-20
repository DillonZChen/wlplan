#ifndef FEATURE_GENERATION_FEATURE_GENERATORS_CCWL_HPP
#define FEATURE_GENERATION_FEATURE_GENERATORS_CCWL_HPP

#include "wl.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

#define NO_EDGE_COLOUR -1

namespace feature_generation {
  class CCWLFeatures : public WLFeatures {
   public:
    CCWLFeatures(const planning::Domain &domain,
                 std::string graph_representation,
                 int iterations,
                 std::string pruning,
                 bool multiset_hash);

    CCWLFeatures(const std::string &filename);

    Embedding embed_impl(const std::shared_ptr<graph::Graph> &graph) override;

    void set_weights(const std::vector<double> &weights);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_FEATURE_GENERATORS_CCWL_HPP
