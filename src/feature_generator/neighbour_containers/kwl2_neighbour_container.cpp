#include "../../../include/feature_generator/neighbour_containers/kwl2_neighbour_container.hpp"

#include <iostream>

namespace feature_generator {
  KWL2NeighbourContainer::KWL2NeighbourContainer(bool multiset_hash)
      : WLNeighbourContainer(multiset_hash) {}

  std::vector<int>
  KWL2NeighbourContainer::get_neighbour_colours(const std::vector<int> &colours) const {
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

  std::vector<int> KWL2NeighbourContainer::remap(const std::vector<int> &input,
                                                 const std::map<int, int> &remap) {
    clear();

    std::vector<int> output = {remap.at(input.at(0))};

    for (const auto &[col0, col1, n_occurrences] : deconstruct(input)) {
      for (int i = 0; i < n_occurrences; i++) {
        insert(remap.at(col0), remap.at(col1));
      }
    }

    std::vector<int> vec = to_vector();
    output.insert(output.end(), vec.begin(), vec.end());

    return output;
  }
}  // namespace feature_generator
