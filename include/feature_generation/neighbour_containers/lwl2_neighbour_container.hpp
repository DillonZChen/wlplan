#ifndef FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_LWL2_NEIGHBOUR_CONTAINER_HPP
#define FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_LWL2_NEIGHBOUR_CONTAINER_HPP

#include "kwl2_neighbour_container.hpp"

#include <map>
#include <set>
#include <utility>

namespace feature_generation {
  class LWL2NeighbourContainer : public KWL2NeighbourContainer {
   public:
    LWL2NeighbourContainer(bool multiset_hash);

    std::vector<int> remap(const std::vector<int> &input, const std::map<int, int> &remap) override;
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_LWL2_NEIGHBOUR_CONTAINER_HPP
