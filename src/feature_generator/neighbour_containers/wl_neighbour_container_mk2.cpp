#include "../../../include/feature_generator/neighbour_containers/wl_neighbour_container_mk2.hpp"

#include <algorithm>
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

    void WLNeighbourContainerMk2::clear_init(size_t s) {
      if (multiset_hash)
        neighbours_multiset.clear(), neighbours_multiset.reserve(s);
      else
        neighbours_set.clear();
    }

    void WLNeighbourContainerMk2::insert(const int node_colour, const int edge_label) {
      const int key = n_relations * node_colour + edge_label;

      if (multiset_hash) {
        neighbours_multiset.push_back(key);
      } else {
        neighbours_set.insert(key);
      }
    }

    std::vector<int> WLNeighbourContainerMk2::to_vector() {
      if (multiset_hash) {
        std::sort(neighbours_multiset.begin(), neighbours_multiset.end());
        return neighbours_multiset;
      } else {
        std::vector<int> vec(neighbours_set.begin(), neighbours_set.end());
        return vec;
      }
    }

    std::vector<std::tuple<int, int, int>>
    WLNeighbourContainerMk2::deconstruct(const std::vector<int> &colours) {
      std::vector<std::tuple<int, int, int>> output;

      if (multiset_hash) {
        std::map<std::pair<int, int>, int> count_map;

        for (const int &key : colours) {
          int edge_label = key % n_relations;
          int node_colour = key / n_relations;
          count_map.try_emplace(std::make_pair(node_colour, edge_label), 0);
          count_map[std::make_pair(node_colour, edge_label)] += 1;
        }

        for (const auto &[key, count] : count_map) {
          output.push_back(std::tuple<int, int, int>(key.first, key.second, count));
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
    WLNeighbourContainerMk2::get_neighbour_colours(const std::vector<int> &colours) {
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
