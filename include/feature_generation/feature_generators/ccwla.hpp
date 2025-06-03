#ifndef FEATURE_GENERATION_FEATURE_GENERATORS_CCWLA_HPP
#define FEATURE_GENERATION_FEATURE_GENERATORS_CCWLA_HPP

// Experimental: add max(a - b, 0) features to CCWL

#include "ccwl.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

#define NO_EDGE_COLOUR -1

namespace feature_generation {
  class CCWLaFeatures : public CCWLFeatures {
   public:
    CCWLaFeatures(const planning::Domain &domain,
                  std::string graph_representation,
                  int iterations,
                  std::string pruning,
                  bool multiset_hash);

    CCWLaFeatures(const std::string &filename);

    CCWLaFeatures(const std::string &filename, bool quiet);

    Embedding embed_impl(const std::shared_ptr<graph::Graph> &graph) override;

    void set_weights(const std::vector<double> &weights);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_FEATURE_GENERATORS_CCWL_HPP
