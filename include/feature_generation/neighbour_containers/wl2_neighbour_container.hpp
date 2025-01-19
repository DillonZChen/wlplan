#ifndef FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL2_NEIGHBOUR_CONTAINER_HPP
#define FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL2_NEIGHBOUR_CONTAINER_HPP

#include "wl_neighbour_container.hpp"

#include <map>
#include <set>
#include <utility>

namespace feature_generation {
  class WL2NeighbourContainer : public WLNeighbourContainer {
    // We use the same keys as relational WL: (multi)set of <node_colour, edge_label> pairs
    // Since these are int-int pairs, we can reuse the same WL code for the 2-WL variants:
    // - for lwl2, these are {col0, col1}, but in implementation we sort so col0 <= col1
    // - for kwl2, there are <col0, col1> i.e. ordered tuples
    // The only difference in this class is getting neighbour colours for pruning

   public:
    WL2NeighbourContainer(bool multiset_hash);

    std::vector<int> get_neighbour_colours(const std::vector<int> &colours) const override;
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL2_NEIGHBOUR_CONTAINER_HPP
