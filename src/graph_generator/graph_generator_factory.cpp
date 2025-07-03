#include "../../include/graph_generator/graph_generator_factory.hpp"

#include "../../include/graph_generator/graph_generators/aoag.hpp"
#include "../../include/graph_generator/graph_generators/ilg.hpp"
#include "../../include/graph_generator/graph_generators/nilg.hpp"
#include "../../include/graph_generator/graph_generators/ploig.hpp"

namespace graph_generator {
  std::shared_ptr<GraphGenerator> init_feature_generator(const std::string &name,
                                                         const planning::Domain &domain) {
    std::shared_ptr<GraphGenerator> graph_generator;
    if (name == "ilg") {
      graph_generator = std::make_shared<ILGGenerator>(domain, false);
    } else if (name == "nilg") {
      graph_generator = std::make_shared<NILGGenerator>(domain, true);
    } else if (name == "ploig") {
      graph_generator = std::make_shared<PLOIGGenerator>(domain, true);
    } else if (name == "aoag") {
      graph_generator = std::make_shared<AOAGGenerator>(domain, true);
    } else if (name == "custom") {
      graph_generator = NULL;
    } else {
      throw std::runtime_error("Unknown graph generator " + name);
    }
    return graph_generator;
  }
}  // namespace graph_generator
