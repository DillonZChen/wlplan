#ifndef FEATURE_GENERATION_FEATURE_GENERATORS_NIWL_HPP
#define FEATURE_GENERATION_FEATURE_GENERATORS_NIWL_HPP

#include "iwl.hpp"

#include <memory>
#include <string>
#include <vector>

namespace feature_generation {
  class NIWLFeatures : public IWLFeatures {
   public:
    NIWLFeatures(const planning::Domain &domain,
                 std::string graph_representation,
                 int iterations,
                 std::string pruning,
                 bool multiset_hash);

    NIWLFeatures(const std::string &filename);

    NIWLFeatures(const std::string &filename, bool quiet);

    Embedding embed_impl(const std::shared_ptr<graph::Graph> &graph) override;
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_FEATURE_GENERATORS_NIWL_HPP
