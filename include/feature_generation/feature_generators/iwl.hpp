#ifndef FEATURE_GENERATION_FEATURE_GENERATORS_IWL_HPP
#define FEATURE_GENERATION_FEATURE_GENERATORS_IWL_HPP

#include "wl.hpp"

#include <memory>
#include <string>
#include <vector>

// use minimum int because negative values are occupied by constant objects
#define INDIVIDUALISE_COLOUR -2147483648

namespace feature_generation {
  class IWLFeatures : public WLFeatures {
   public:
    IWLFeatures(const std::string feature_name,
                const planning::Domain &domain,
                std::string graph_representation,
                int iterations,
                std::string pruning,
                bool multiset_hash);

    IWLFeatures(const planning::Domain &domain,
                std::string graph_representation,
                int iterations,
                std::string pruning,
                bool multiset_hash);

    IWLFeatures(const std::string &filename);

    IWLFeatures(const std::string &filename, bool quiet);

    Embedding embed_impl(const std::shared_ptr<graph::Graph> &graph) override;

   protected:
    void collect_impl(const std::vector<graph::Graph> &graphs) override;
    void refine(const std::shared_ptr<graph::Graph> &graph,
                std::vector<int> &colours,
                int iteration);
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_FEATURE_GENERATORS_IWL_HPP
