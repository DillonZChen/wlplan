#ifndef FEATURE_GENERATOR_FEATURE_GENERATORS_CCWLA_HPP
#define FEATURE_GENERATOR_FEATURE_GENERATORS_CCWLA_HPP

#include "ccwl.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

#define NO_EDGE_COLOUR -1

namespace wlplan {
  namespace feature_generator {
    class CCWLaFeatures : public CCWLFeatures {
     public:
      CCWLaFeatures(const planning::Domain &domain,
                    std::string graph_representation,
                    int iterations,
                    std::string pruning,
                    bool multiset_hash);

      CCWLaFeatures(const std::string &filename);

      CCWLaFeatures(const std::string &filename, bool quiet);

      int get_n_features() const override;

      Embedding embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) override;
    };
  }  // namespace feature_generator
}  // namespace wlplan

#endif  // FEATURE_GENERATOR_FEATURE_GENERATORS_CCWL_HPP
