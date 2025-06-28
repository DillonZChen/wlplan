#ifndef GRAPH_GRAPH_GENERATORS_AOAG_HPP
#define GRAPH_GRAPH_GENERATORS_AOAG_HPP

// Reference:
// Ryan X. Wang, Felipe Trevizan. Leveraging Action Relational Structures for Integrated Learning
// and Planning. In ICAPS, 2025.

// Link:
// https://arxiv.org/abs/2504.20318

#include "ilg.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace graph {
  class AOAGGenerator : public ILGGenerator {
   public:
    AOAGGenerator(const planning::Domain &domain, bool differentiate_constant_objects);

    // Extends ILG methods
    std::shared_ptr<Graph> to_graph(const planning::State &state) override;
    std::shared_ptr<Graph> to_graph(const planning::State &state,
                                    const std::vector<planning::Action> &actions);
    std::shared_ptr<Graph> to_graph_opt(const planning::State &state) override;
    void reset_graph() const override;
  };
}  // namespace graph

#endif  // GRAPH_GRAPH_GENERATORS_AOAG_HPP
