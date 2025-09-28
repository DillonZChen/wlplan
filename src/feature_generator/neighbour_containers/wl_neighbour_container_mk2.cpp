#include "../../../include/feature_generator/neighbour_containers/wl_neighbour_container_mk2.hpp"

#include <iostream>

namespace wlplan {
  namespace feature_generator {
    WLNeighbourContainerMk2::WLNeighbourContainerMk2(bool multiset_hash,
                                                     int n_features,
                                                     int n_relations)
        : NeighbourContainer(multiset_hash), n_features(n_features), n_relations(n_relations) {}

    void WLNeighbourContainerMk2::clear() {
      if (multiset_hash)
        neighbours_multiset.clear();
      else
        neighbours_set.clear();
    }

    void WLNeighbourContainerMk2::insert(const int node_colour, const int edge_label) {
      const int key = n_relations * node_colour + edge_label;

      if (multiset_hash) {
        auto it = neighbours_multiset.lower_bound(key);
        if (it != neighbours_multiset.end() && it->first == key) {
          // Key already exists, update it.
          it->second++;
        } else {
          // Key does not exist, insert it with a hint.
          neighbours_multiset.emplace_hint(it, key, 1);
        }
      } else {
        neighbours_set.insert(key);
      }
    }

    std::vector<int> WLNeighbourContainerMk2::to_vector() const {
      if (multiset_hash) {
        std::vector<int> vec;
        vec.reserve(neighbours_multiset.size() * 2);
        // neighbours is already sorted by key due to std::map
        for (const auto &[label_colour, count] : neighbours_multiset) {
          vec.push_back(label_colour);
          vec.push_back(count);
        }
        return vec;
      } else {
        std::vector<int> vec(neighbours_set.begin(), neighbours_set.end());
        return vec;
      }
    }

    std::vector<std::tuple<int, int, int>>
    WLNeighbourContainerMk2::deconstruct(const std::vector<int> &colours) const {
      std::vector<std::tuple<int, int, int>> output;

      if (multiset_hash) {
        int inc = 2;

        if (colours.size() % inc != 1) {
          throw std::runtime_error("Key " + to_string(colours) + " has size() % " +
                                   std::to_string(inc) +
                                   " != 1 for multiset_hash=" + std::to_string(multiset_hash));
        }

        for (int i = 0; i < (int)colours.size() - inc; i += inc) {
          int edge_label = colours.at(i) % n_relations;
          int node_colour = colours.at(i) / n_relations;
          int n_occurrences = colours.at(i + 1);
          output.push_back(std::tuple<int, int, int>(node_colour, edge_label, n_occurrences));
        }
      } else {
        for (const int &key : colours) {
          int edge_label = key % n_relations;
          int node_colour = key / n_relations;
          int n_occurrences = 1;  // In the set case, each key is unique
          output.push_back(std::tuple<int, int, int>(node_colour, edge_label, n_occurrences));
        }
      }

      return output;
    }

    std::vector<int>
    WLNeighbourContainerMk2::get_neighbour_colours(const std::vector<int> &colours) const {
      std::vector<int> neighbour_colours;
      for (const auto &[node_colour, edge_label, n_occurrences] : deconstruct(colours)) {
        neighbour_colours.push_back(node_colour);
      }
      neighbour_colours.push_back(colours.at(colours.size() - 1));
      return neighbour_colours;
    }

    std::vector<int> WLNeighbourContainerMk2::remap(const std::vector<int> &input,
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
