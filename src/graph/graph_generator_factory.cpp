#include "../../include/graph/graph_generator_factory.hpp"

#include "../../include/graph/graph_generators/ilg.hpp"
#include "../../include/graph/graph_generators/nilg.hpp"
#include "../../include/graph/graph_generators/ploig.hpp"

namespace graph {
  std::shared_ptr<GraphGenerator> create_graph_generator(const std::string &name,
                                                         const planning::Domain &domain) {
    std::shared_ptr<GraphGenerator> graph_generator;
    // std::cout << "Constructing graph generator " << name << std::endl;
    if (name == "ilg") {
      graph_generator = std::make_shared<ILGGenerator>(domain, false);
    } else if (name == "nilg") {
      graph_generator = std::make_shared<NILGGenerator>(domain, true);
    } else if (name == "ploig") {
      graph_generator = std::make_shared<PLOIGGenerator>(domain, true);
    } else if (name == "custom") {
      graph_generator = NULL;
    } else {
      throw std::runtime_error("Unknown graph generator " + name);
    }
    // std::cout << "Finished constructing graph generator." << std::endl;
    return graph_generator;
  }
}  // namespace graph
