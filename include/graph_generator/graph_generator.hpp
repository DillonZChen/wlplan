#ifndef GRAPH_GENERATOR_GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_GRAPH_GENERATOR_HPP

#include "../data/dataset.hpp"
#include "../planning/action.hpp"
#include "../planning/atom.hpp"
#include "../planning/domain.hpp"
#include "../planning/problem.hpp"
#include "../planning/state.hpp"
#include "graph.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace graph_generator {
  class GraphGenerator {
   public:
    virtual ~GraphGenerator() = default;

    std::vector<graph_generator::Graph> to_graphs(const data::DomainDataset dataset);

    // Change the base graph based on the input problem
    virtual void set_problem(const planning::Problem &problem) = 0;

    // Makes a copy of the base graph and makes the necessary modifications. Assumes the state is
    // from the problem that is set but does not check this.
    virtual std::shared_ptr<Graph> to_graph(const planning::State &state) = 0;
    // Default implementation for action inputs.
    std::shared_ptr<Graph> to_graph(const planning::State &state,
                                    const std::vector<planning::Action> &actions);

    // Optimised variant of to_graph() but requires calling reset_graph() after. Does not make a
    // copy of the base graph and instead modifies it directly, and undoing the modifications with
    // reset_graph(). Only supported for state-based graphs. Not yet supported for action graphs.
    // To skip this implementation, you can use to_graph(state) and make reset_graph() blank.
    virtual std::shared_ptr<Graph> to_graph_opt(const planning::State &state) = 0;
    virtual void reset_graph() const = 0;

    void print_init_colours() const;

   protected:
    // Do not use a vector here because colours can be negative, i.e. constant objects
    std::map<int, std::string> colour_to_description;
  };
}  // namespace graph_generator

#endif  // GRAPH_GENERATOR_GRAPH_GENERATOR_HPP
