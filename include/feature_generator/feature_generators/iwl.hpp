#ifndef FEATURE_GENERATOR_FEATURE_GENERATORS_IWL_HPP
#define FEATURE_GENERATOR_FEATURE_GENERATORS_IWL_HPP

#include "wl.hpp"

#include <memory>
#include <string>
#include <vector>

// use minimum int because negative values are occupied by constant objects
#define INDIVIDUALISE_COLOUR -2147483648

namespace feature_generator {
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

    std::unordered_map<int, int> collect_embed(const planning::State &state) override;
    Embedding embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) override;

   protected:
    void collect_impl(const std::vector<graph_generator::Graph> &graphs) override;
    void refine(const std::shared_ptr<graph_generator::Graph> &graph,
                std::vector<int> &colours,
                int iteration);
  };
}  // namespace feature_generator

#endif  // FEATURE_GENERATOR_FEATURE_GENERATORS_IWL_HPP
