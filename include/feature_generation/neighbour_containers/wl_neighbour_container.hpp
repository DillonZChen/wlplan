#ifndef FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL_NEIGHBOUR_CONTAINER_HPP
#define FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL_NEIGHBOUR_CONTAINER_HPP

#include "../neighbour_container.hpp"

#include <map>
#include <set>
#include <tuple>

namespace feature_generation {
  class WLNeighbourContainer : public NeighbourContainer {
   public:
    WLNeighbourContainer(bool multiset_hash);

    void clear() override;
    void insert(const int node_colour, const int edge_label) override;
    std::vector<int> to_vector() const override;

    // triples are <node_colour, edge_label, n_occurrence>
    std::vector<std::tuple<int, int, int>> deconstruct(const std::vector<int> &colours) const;
    std::vector<int> get_neighbour_colours(const std::vector<int> &colours) const override;

    std::vector<int> remap(const std::vector<int> &input, const std::map<int, int> &remap) override;

   protected:
    // profiling showed that using pairs is faster than vector of maps/sets, and that ordered
    // containers are faster than unordered containers given that pair does not have a hash
    std::set<std::pair<int, int>> neighbours_set;
    std::map<std::pair<int, int>, int> neighbours_mset;
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_NEIGHBOUR_CONTAINERS_WL_NEIGHBOUR_CONTAINER_HPP
