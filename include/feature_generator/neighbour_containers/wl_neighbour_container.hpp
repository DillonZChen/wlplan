#ifndef FEATURE_GENERATOR_NEIGHBOUR_CONTAINERS_WL_NEIGHBOUR_CONTAINER_HPP
#define FEATURE_GENERATOR_NEIGHBOUR_CONTAINERS_WL_NEIGHBOUR_CONTAINER_HPP

#include "../neighbour_container.hpp"

#include <map>
#include <set>
#include <tuple>

namespace wlplan {
  namespace feature_generator {
    class WLNeighbourContainer : public NeighbourContainer {
     public:
      WLNeighbourContainer(bool multiset_hash);

      void clear() override;
      void clear_init(size_t s) override;
      void insert(const int node_colour, const int edge_label) override;
      std::vector<int> to_vector() override;

      // triples are <node_colour, edge_label, n_occurrence>
      std::vector<std::tuple<int, int, int>> deconstruct(const std::vector<int> &colours);
      std::vector<int> get_neighbour_colours(const std::vector<int> &colours) override;

      std::vector<int> remap(const std::vector<int> &input,
                             const std::map<int, int> &remap) override;

     protected:
      std::map<std::pair<int, int>, int> neighbours;
    };
  }  // namespace feature_generator
}  // namespace wlplan

#endif  // FEATURE_GENERATOR_NEIGHBOUR_CONTAINERS_WL_NEIGHBOUR_CONTAINER_HPP
