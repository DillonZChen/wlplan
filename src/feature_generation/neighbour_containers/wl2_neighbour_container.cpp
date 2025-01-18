#include "../../../include/feature_generation/neighbour_containers/wl2_neighbour_container.hpp"

#include <iostream>

namespace feature_generation {
  WL2NeighbourContainer::WL2NeighbourContainer(bool multiset_hash)
      : NeighbourContainer(multiset_hash) {}

  void WL2NeighbourContainer::clear() {
    if (multiset_hash) {
      neighbours_mset.clear();
    } else {
      neighbours_set.clear();
    }
  }

  void WL2NeighbourContainer::insert(const int colour) {
    if (multiset_hash) {
      if (neighbours_mset.count(colour) > 0)
        neighbours_mset[colour]++;
      else
        neighbours_mset[colour] = 1;
    } else {
      neighbours_set.insert(colour);
    }
  }

  void WL2NeighbourContainer::insert(const int colour1, const int colour2) {
    insert(colour1);
    insert(colour2);
  }

  std::vector<int> WL2NeighbourContainer::to_vector() const {
    std::vector<int> vec;
    if (multiset_hash) {
      for (const auto &[colour, count] : neighbours_mset) {
        vec.push_back(colour);
        vec.push_back(count);
      }
    } else {
      for (const auto &colour : neighbours_set) {
        vec.push_back(colour);
      }
    }
    return vec;
  }

  std::vector<std::pair<int, int>>
  WL2NeighbourContainer::deconstruct(const std::vector<int> &colours) const {
    std::vector<std::pair<int, int>> output;

    int inc;
    if (multiset_hash) {
      inc = 2;
    } else {
      inc = 1;
    }

    if (colours.size() % inc != 1) {
      std::cout << "ERROR: key " << to_string(colours) << " has size() % " << inc
                << " != 1 for multiset_hash=" << multiset_hash << std::endl;
      exit(-1);
    }

    for (size_t i = 1; i < colours.size(); i += inc) {
      int node_colour = colours.at(i);
      int n_occurrences;
      if (multiset_hash) {
        n_occurrences = colours.at(i + 1);
      } else {
        n_occurrences = 1;
      }
      output.push_back(std::pair<int, int>(node_colour, n_occurrences));
    }

    return output;
  }

  std::vector<int>
  WL2NeighbourContainer::get_neighbour_colours(const std::vector<int> &colours) const {
    std::vector<int> neighbour_colours = {colours.at(0)};
    for (const auto &[node_colour, n_occurrences] : deconstruct(colours)) {
      neighbour_colours.push_back(node_colour);
    }
    return neighbour_colours;
  }

  std::vector<int> WL2NeighbourContainer::remap(const std::vector<int> &input,
                                                const std::map<int, int> &remap) {
    clear();

    std::vector<int> output = {remap.at(input.at(0))};

    for (const auto &[node_colour, n_occurrences] : deconstruct(input)) {
      for (int i = 0; i < n_occurrences; i++) {
        insert(remap.at(node_colour));
      }
    }

    std::vector<int> vec = to_vector();
    output.insert(output.end(), vec.begin(), vec.end());

    return output;
  }
}  // namespace feature_generation
