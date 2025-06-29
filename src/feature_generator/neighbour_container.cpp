#include "../../include/feature_generator/neighbour_container.hpp"

namespace feature_generator {
  NeighbourContainer::NeighbourContainer(bool multiset_hash) : multiset_hash(multiset_hash) {}

  std::string NeighbourContainer::to_string(const std::vector<int> &colours) const {
    std::string str = "";
    for (const int i : colours) {
      str += "." + std::to_string(i);
    }
    return str;
  }

  std::string NeighbourContainer::to_string() const {
    std::vector<int> vec = to_vector();
    return to_string(vec);
  }

}  // namespace feature_generator
