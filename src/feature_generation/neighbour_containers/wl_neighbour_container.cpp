#include "../../../include/feature_generation/neighbour_containers/wl_neighbour_container.hpp"

#include <iostream>

namespace feature_generation {
  WLNeighbourContainer::WLNeighbourContainer(bool multiset_hash)
      : NeighbourContainer(multiset_hash) {}

  void WLNeighbourContainer::clear() {
    if (multiset_hash) {
      neighbours_mset.clear();
    } else {
      neighbours_set.clear();
    }
  }

  void WLNeighbourContainer::insert(const int node_colour, const int edge_label) {
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

  std::vector<int> WLNeighbourContainer::to_vector() const {
    std::vector<int> vec;
    if (multiset_hash) {
      for (const auto &kv : neighbours_mset) {
        vec.push_back(kv.first.first);   // edge label
        vec.push_back(kv.first.second);  // node colour
        vec.push_back(kv.second);        // count in multiset
      }
    } else {
      for (const auto &kv : neighbours_set) {
        vec.push_back(kv.first);   // edge label
        vec.push_back(kv.second);  // node colour
      }
    }
    return vec;
  }

  std::vector<std::tuple<int, int, int>>
  WLNeighbourContainer::deconstruct(const std::vector<int> &colours) const {
    std::vector<std::tuple<int, int, int>> output;

    int inc;
    if (multiset_hash) {
      inc = 3;
    } else {
      inc = 2;
    }

    if (colours.size() % inc != 1) {
      std::cout << "ERROR: key " << to_string(colours) << " has size() % " << inc
                << " != 1 for multiset_hash=" << multiset_hash << std::endl;
      exit(-1);
    }

    for (size_t i = 1; i < colours.size(); i += inc) {
      int edge_label = colours.at(i);
      int node_colour = colours.at(i + 1);
      int n_occurrences;
      if (multiset_hash) {
        n_occurrences = colours.at(i + 2);
      } else {
        n_occurrences = 1;
      }
      output.push_back(std::tuple<int, int, int>(node_colour, edge_label, n_occurrences));
    }

    return output;
  }

  std::vector<int>
  WLNeighbourContainer::get_neighbour_colours(const std::vector<int> &colours) const {
    std::vector<int> neighbour_colours = {colours.at(0)};
    for (const auto &[node_colour, edge_label, n_occurrences] : deconstruct(colours)) {
      neighbour_colours.push_back(node_colour);
    }
    return neighbour_colours;
  }

  std::vector<int> WLNeighbourContainer::remap(const std::vector<int> &input,
                                               const std::map<int, int> &remap) {
    clear();

    std::vector<int> output = {remap.at(input.at(0))};

    for (const auto &[node_colour, edge_label, n_occurrences] : deconstruct(input)) {
      for (int i = 0; i < n_occurrences; i++) {
        insert(remap.at(node_colour), edge_label);
      }
    }

    std::vector<int> vec = to_vector();
    output.insert(output.end(), vec.begin(), vec.end());

    return output;
  }
}  // namespace feature_generation
