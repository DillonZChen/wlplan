#include "../../include/feature_generation/neighbour_container.hpp"

namespace feature_generation {
  NeighbourContainer::NeighbourContainer(bool multiset_hash)
      : multiset_hash(multiset_hash) {}

  void NeighbourContainer::clear() {
    if (multiset_hash) {
      neighbours_mset.clear();
    } else {
      neighbours_set.clear();
    }
  }

  void NeighbourContainer::insert(const int node_colour, const int edge_label) {
    const std::pair<int, int> key = std::make_pair(edge_label, node_colour);
    if (multiset_hash) {
      if (neighbours_mset.count(key) > 0)
        neighbours_mset[key]++;
      else
        neighbours_mset[key] = 1;
    } else {
      neighbours_set.insert(key);
    }
  }

  std::string NeighbourContainer::to_string() const {
    std::string str = "";
    if (multiset_hash) {
      for (const auto &kv : neighbours_mset) {
        str += ";" + std::to_string(kv.first.first);
        str += "," + std::to_string(kv.first.second);
        str += "x" + std::to_string(kv.second);  // count in multiset
      }
    } else {
      for (const auto &kv : neighbours_set) {
        str += ";" + std::to_string(kv.first);
        str += "," + std::to_string(kv.second);
      }
    }
    return str;
  }
}  // namespace feature_generation
