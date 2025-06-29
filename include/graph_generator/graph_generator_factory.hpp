#ifndef GRAPH_GENERATOR_GRAPH_GENERATOR_FACTORY_HPP
#define GRAPH_GENERATOR_GRAPH_GENERATOR_FACTORY_HPP

#include "../planning/domain.hpp"
#include "graph_generator.hpp"

#include <memory>
#include <string>

namespace graph_generator {
  std::shared_ptr<GraphGenerator> init_feature_generator(const std::string &name,
                                                         const planning::Domain &domain);

  std::shared_ptr<GraphGenerator> init_feature_generator(const std::string &name,
                                                         const planning::Domain &domain,
                                                         bool differentiate_constant_objects);
}  // namespace graph_generator

#endif  // GRAPH_GENERATOR_GRAPH_GENERATOR_FACTORY_HPP
