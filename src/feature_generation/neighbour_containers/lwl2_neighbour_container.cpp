#include "../../../include/feature_generation/neighbour_containers/lwl2_neighbour_container.hpp"

#include <iostream>

namespace feature_generation {
  LWL2NeighbourContainer::LWL2NeighbourContainer(bool multiset_hash)
      : KWL2NeighbourContainer(multiset_hash) {}

  std::vector<int> LWL2NeighbourContainer::remap(const std::vector<int> &input,
                                                 const std::map<int, int> &remap) {
    clear();

    std::vector<int> output = {remap.at(input.at(0))};

    for (const auto &[col0, col1, n_occurrences] : deconstruct(input)) {
      for (int i = 0; i < n_occurrences; i++) {
        int col_a = std::min(remap.at(col0), remap.at(col1));
        int col_b = std::max(remap.at(col0), remap.at(col1));
        insert(col_a, col_b);
      }
    }

    std::vector<int> vec = to_vector();
    output.insert(output.end(), vec.begin(), vec.end());

    return output;
  }
}  // namespace feature_generation
