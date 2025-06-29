#ifndef FEATURE_GENERATOR_FEATURE_GENERATORS_NIWL_HPP
#define FEATURE_GENERATOR_FEATURE_GENERATORS_NIWL_HPP

#include "iwl.hpp"

#include <memory>
#include <string>
#include <vector>

namespace feature_generator {
  class NIWLFeatures : public IWLFeatures {
   public:
    NIWLFeatures(const planning::Domain &domain,
                 std::string graph_representation,
                 int iterations,
                 std::string pruning,
                 bool multiset_hash);

    NIWLFeatures(const std::string &filename);

    NIWLFeatures(const std::string &filename, bool quiet);

    Embedding embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) override;
  };
}  // namespace feature_generator

#endif  // FEATURE_GENERATOR_FEATURE_GENERATORS_NIWL_HPP
