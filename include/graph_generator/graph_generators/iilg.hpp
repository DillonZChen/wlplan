#ifndef GRAPH_GENERATOR_GRAPH_GENERATORS_IILG_HPP
#define GRAPH_GENERATOR_GRAPH_GENERATORS_IILG_HPP

#include "ilg.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wlplan::graph_generator {
  class IILGGenerator : public ILGGenerator {
   public:
    IILGGenerator(const planning::Domain &domain);

    // Graph generation
    void set_problem(const planning::Problem &problem) override;

   protected:
    std::unordered_map<std::string, int> obj_to_colour;
  };
}  // namespace wlplan::graph_generator

#endif  // GRAPH_GENERATOR_GRAPH_GENERATORS_IILG_HPP
