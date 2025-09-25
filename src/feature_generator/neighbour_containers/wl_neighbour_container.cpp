#include "../../../include/feature_generator/neighbour_containers/wl_neighbour_container.hpp"

#include <iostream>

namespace wlplan {
  namespace feature_generator {
    WLNeighbourContainer::WLNeighbourContainer(bool multiset_hash)
        : NeighbourContainer(multiset_hash) {}

    void WLNeighbourContainer::clear() { neighbours.clear(); }

    void WLNeighbourContainer::insert(const int node_colour, const int edge_label) {
      const auto key = std::make_pair(edge_label, node_colour);

      if (multiset_hash) {
        auto it = neighbours.lower_bound(key);
        if (it != neighbours.end() && it->first == key) {
          // Key already exists, update it.
          it->second++;
        } else {
          // Key does not exist, insert it with a hint.
          neighbours.emplace_hint(it, key, 1);
        }
      } else {
        neighbours[key] = 1;
      }
    }

    std::vector<int> WLNeighbourContainer::to_vector() const {
      std::vector<int> vec;
      vec.reserve(neighbours.size() * 3);
      for (const auto &[label_colour, count] : neighbours) {
        vec.push_back(label_colour.first);   // edge label
        vec.push_back(label_colour.second);  // node colour
        vec.push_back(count);                // count in multiset (always 1 for set)
      }
      return vec;
    }

    std::vector<std::tuple<int, int, int>>
    WLNeighbourContainer::deconstruct(const std::vector<int> &colours) const {
      std::vector<std::tuple<int, int, int>> output;

      int inc = 3;

      if (colours.size() % inc != 1) {
        throw std::runtime_error("Key " + to_string(colours) + " has size() % " +
                                 std::to_string(inc) +
                                 " != 1 for multiset_hash=" + std::to_string(multiset_hash));
      }

      for (int i = 0; i < (int)colours.size() - inc; i += inc) {
        int edge_label = colours.at(i);
        int node_colour = colours.at(i + 1);
        int n_occurrences = colours.at(i + 2);
        output.push_back(std::tuple<int, int, int>(node_colour, edge_label, n_occurrences));
      }

      return output;
    }

    std::vector<int>
    WLNeighbourContainer::get_neighbour_colours(const std::vector<int> &colours) const {
      std::vector<int> neighbour_colours;
      for (const auto &[node_colour, edge_label, n_occurrences] : deconstruct(colours)) {
        neighbour_colours.push_back(node_colour);
      }
      neighbour_colours.push_back(colours.at(colours.size() - 1));
      return neighbour_colours;
    }

    std::vector<int> WLNeighbourContainer::remap(const std::vector<int> &input,
                                                 const std::map<int, int> &remap) {
      clear();

      for (const auto &[node_colour, edge_label, n_occurrences] : deconstruct(input)) {
        for (int i = 0; i < n_occurrences; i++) {
          insert(remap.at(node_colour), edge_label);
        }
      }

      std::vector<int> output = to_vector();
      output.push_back(remap.at(input.at(input.size() - 1)));

      return output;
    }
  }  // namespace feature_generator
}  // namespace wlplan
