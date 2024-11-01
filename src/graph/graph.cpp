#include "../../include/graph/graph.hpp"

namespace graph {
  Graph::Graph(const std::vector<int> &node_colours,
               const std::vector<double> &node_values,
               const std::vector<std::string> &node_names,
               const std::vector<std::vector<std::pair<int, int>>> &edges)
      : nodes(node_colours),
        node_values(node_values),
        edges(edges),
        store_node_names(true),
        index_to_node_(node_names) {
    for (size_t u = 0; u < node_names.size(); u++) {
      node_to_index_[node_names[u]] = u;
    }
  }

  Graph::Graph(const std::vector<int> &node_colours,
               const std::vector<double> &node_values,
               const std::vector<std::vector<std::pair<int, int>>> &edges)
      : Graph(node_colours, node_values, std::vector<std::string>(), edges) {}

  Graph::Graph(const std::vector<int> &node_colours,
               const std::vector<std::string> &node_names,
               const std::vector<std::vector<std::pair<int, int>>> &edges)
      : Graph(node_colours, std::vector<double>(), node_names, edges) {}

  Graph::Graph(const std::vector<int> &node_colours,
               const std::vector<std::vector<std::pair<int, int>>> &edges)
      : Graph(node_colours, std::vector<double>(), edges) {}

  Graph::Graph(bool store_node_names) : store_node_names(store_node_names) {}

  int Graph::add_node(const std::string &node_name, int colour, double value) {
    int index = nodes.size();
    nodes.push_back(colour);
    node_values.push_back(value);
    if (store_node_names) {
      node_to_index_[node_name] = index;
      index_to_node_.push_back(node_name);
    }
    edges.push_back(std::vector<std::pair<int, int>>());
    return index;
  }

  int Graph::add_node(const std::string &node_name, int colour) {
    return add_node(node_name, colour, 0);
  }

  void Graph::add_edge(const int u, const int r, const int v) {
    edges.at(u).push_back(std::make_pair(r, v));
  }

  void Graph::add_edge(const std::string &u_name, const int r, const std::string &v_name) {
    if (!store_node_names) {
      std::cout << "Error: cannot add edge by name as store_node_names is false" << std::endl;
      exit(-1);
    }
    add_edge(node_to_index_.at(u_name), r, node_to_index_.at(v_name));
  }

  void Graph::change_node_colour(const int u, const int new_colour) { nodes[u] = new_colour; }

  void Graph::change_node_colour(const std::string &node_name, const int new_colour) {
    if (!store_node_names) {
      std::cout << "Error: cannot change node colour by name as store_node_names is false"
                << std::endl;
      exit(-1);
    }
    change_node_colour(node_to_index_.at(node_name), new_colour);
  }

  void Graph::change_node_value(const int u, const double new_value) { node_values[u] = new_value; }

  void Graph::change_node_value(const std::string &node_name, const double new_value) {
    if (!store_node_names) {
      std::cout << "Error: cannot change node value by name as store_node_names is false"
                << std::endl;
      exit(-1);
    }
    change_node_value(node_to_index_.at(node_name), new_value);
  }

  std::vector<std::set<int>> Graph::get_node_to_neighbours() const {
    std::vector<std::set<int>> node_to_neighbours(nodes.size());
    for (size_t u = 0; u < nodes.size(); u++) {
      for (size_t j = 0; j < edges[u].size(); j++) {
        node_to_neighbours[u].insert(edges[u][j].second);
      }
    }
    return node_to_neighbours;
  }

  std::string Graph::get_node_name(const int u) const {
    if (!store_node_names) {
      std::cout << "Error: cannot get node name as store_node_names is false" << std::endl;
      exit(-1);
    }
    return index_to_node_.at(u);
  }

  int Graph::get_node_index(const std::string &node_name) const {
    return node_to_index_.at(node_name);
  }

  int Graph::get_n_nodes() const { return nodes.size(); }

  int Graph::get_n_edges() const {
    int ret = 0;
    for (size_t u = 0; u < nodes.size(); u++) {
      ret += edges.at(u).size();
    }
    return ret;
  }

  std::string Graph::to_string() const {
    std::string ret = "<Graph with " + std::to_string(nodes.size()) + " nodes and " +
                      std::to_string(get_n_edges()) + " edges>";
    return ret;
  }

  void Graph::dump() const {
    std::cout << nodes.size() << " nodes" << std::endl;
    for (size_t u = 0; u < nodes.size(); u++) {
      std::cout << u;
      if (store_node_names) {
        std::cout << " : " << get_node_name(u);
      }
      std::cout << " : " << nodes[u] << std::endl;
    }

    std::cout << get_n_edges() << " edges" << std::endl;
    for (size_t u = 0; u < nodes.size(); u++) {
      for (size_t j = 0; j < edges[u].size(); j++) {
        std::cout << u << " " << edges[u][j].first << " " << edges[u][j].second << std::endl;
      }
    }
  }
}  // namespace graph
