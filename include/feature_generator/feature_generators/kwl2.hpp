#ifndef FEATURE_GENERATOR_FEATURE_GENERATORS_KWL2_HPP
#define FEATURE_GENERATOR_FEATURE_GENERATORS_KWL2_HPP

#include "../features.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

#define NO_EDGE_COLOUR -1

namespace wlplan {
  namespace feature_generator {
    class KWL2Features : public Features {
     public:
      KWL2Features(const std::string wl_name,
                   const planning::Domain &domain,
                   std::string graph_representation,
                   int iterations,
                   std::string pruning,
                   bool multiset_hash);

      KWL2Features(const planning::Domain &domain,
                   std::string graph_representation,
                   int iterations,
                   std::string pruning,
                   bool multiset_hash);

      KWL2Features(const std::string &filename);

      KWL2Features(const std::string &filename, bool quiet);

      int get_n_features() const override;

      Embedding embed_impl(const std::shared_ptr<graph_generator::Graph> &graph) override;

     protected:
      inline int get_initial_colour(int index,
                                    int u,
                                    int v,
                                    const std::shared_ptr<graph_generator::Graph> &graph,
                                    const std::vector<int> &pair_to_edge_label);
      void collect_impl(const std::vector<graph_generator::Graph> &graphs) override;
      void refine(const std::shared_ptr<graph_generator::Graph> &graph,
                  std::vector<int> &colours,
                  int iteration);
    };
  }  // namespace feature_generator
}  // namespace wlplan

#endif  // FEATURE_GENERATOR_FEATURE_GENERATORS_KWL2_HPP
