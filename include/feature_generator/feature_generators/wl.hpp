#ifndef FEATURE_GENERATOR_FEATURE_GENERATORS_WL_HPP
#define FEATURE_GENERATOR_FEATURE_GENERATORS_WL_HPP

#include "../features.hpp"

#include <memory>
#include <string>
#include <vector>

namespace wlplan {
  namespace feature_generator {
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

      WLFeatures(const std::string &filename, bool quiet);

      int get_n_features() const override;

      std::unordered_map<int, int> collect_embed(const planning::State &state) override;
      Embedding embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) override;

     protected:
      void collect_impl(const std::vector<graph_generator::Graph> &graphs) override;
      void refine(const std::shared_ptr<graph_generator::Graph> &graph,
                  std::set<int> &nodes,
                  std::vector<int> &colours,
                  int iteration);
    };
  }  // namespace feature_generator
}  // namespace wlplan

#endif  // FEATURE_GENERATOR_FEATURE_GENERATORS_WL_HPP
