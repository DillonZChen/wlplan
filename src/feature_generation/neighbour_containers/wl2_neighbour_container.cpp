#include "../../../include/feature_generation/neighbour_containers/wl2_neighbour_container.hpp"

#include <iostream>

namespace feature_generation {
  WL2NeighbourContainer::WL2NeighbourContainer(bool multiset_hash)
      : WLNeighbourContainer(multiset_hash) {}

  std::vector<int>
  WL2NeighbourContainer::get_neighbour_colours(const std::vector<int> &colours) const {
    std::set<int> neighbour_colours_set;
    for (const auto &[col0, col1, n_occurrences] : deconstruct(colours)) {
      neighbour_colours_set.insert(col0);
      neighbour_colours_set.insert(col1);
    }

    std::vector<int> neighbour_colours = {colours.at(0)};
    neighbour_colours.insert(
        neighbour_colours.end(), neighbour_colours_set.begin(), neighbour_colours_set.end());

    return neighbour_colours;
  }
}  // namespace feature_generation
