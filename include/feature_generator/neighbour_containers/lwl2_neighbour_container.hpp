#ifndef FEATURE_GENERATOR_NEIGHBOUR_CONTAINERS_LWL2_NEIGHBOUR_CONTAINER_HPP
#define FEATURE_GENERATOR_NEIGHBOUR_CONTAINERS_LWL2_NEIGHBOUR_CONTAINER_HPP

#include "kwl2_neighbour_container.hpp"

#include <map>
#include <set>
#include <utility>

namespace feature_generator {
  class LWL2NeighbourContainer : public KWL2NeighbourContainer {
   public:
    LWL2NeighbourContainer(bool multiset_hash);

    std::vector<int> remap(const std::vector<int> &input, const std::map<int, int> &remap) override;
  };
}  // namespace feature_generator

#endif  // FEATURE_GENERATOR_NEIGHBOUR_CONTAINERS_LWL2_NEIGHBOUR_CONTAINER_HPP
