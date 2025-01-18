#ifndef FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL2_NEIGHBOUR_CONTAINER_HPP
#define FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL2_NEIGHBOUR_CONTAINER_HPP

#include "../neighbour_container.hpp"

#include <map>
#include <set>
#include <utility>

namespace feature_generation {
  class WL2NeighbourContainer : public NeighbourContainer {
   public:
    WL2NeighbourContainer(bool multiset_hash);

    void clear() override;
    void insert(const int colour);
    void insert(const int colour0, const int colour1) override;
    std::vector<int> to_vector() const override;

    // pairs are <node_colour, n_occurrence>
    std::vector<std::pair<int, int>> deconstruct(const std::vector<int> &colours) const;
    std::vector<int> get_neighbour_colours(const std::vector<int> &colours) const override;

    std::vector<int> remap(const std::vector<int> &input, const std::map<int, int> &remap) override;

   private:
    std::set<int> neighbours_set;
    std::map<int, int> neighbours_mset;
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL2_NEIGHBOUR_CONTAINER_HPP
