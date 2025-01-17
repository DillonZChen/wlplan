#include "../../include/feature_generation/neighbour_container.hpp"

namespace feature_generation {
  NeighbourContainer::NeighbourContainer(bool multiset_hash) : multiset_hash(multiset_hash) {}

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
    for (const int i : to_vector()) {
      str += "." + std::to_string(i);
    }
    return str;
  }

  std::vector<int> NeighbourContainer::to_vector() const {
    std::vector<int> vec;
    if (multiset_hash) {
      for (const auto &kv : neighbours_mset) {
        vec.push_back(kv.first.first);  // edge label
        vec.push_back(kv.first.second);  // node colour
        vec.push_back(kv.second);  // count in multiset
      }
    } else {
      for (const auto &kv : neighbours_set) {
        vec.push_back(kv.first);  // edge label
        vec.push_back(kv.second);  // node colour
      }
    }
    return vec;
  }
}  // namespace feature_generation
