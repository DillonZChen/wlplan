#include "../../include/feature_generation/pruning_options.hpp"

namespace feature_generation {
  const std::string PruningOptions::NONE = "none";
  const std::string PruningOptions::ALL_MAXSAT = "a-m";
  const std::string PruningOptions::LAYER_GREEDY = "i-g";
  const std::string PruningOptions::LAYER_MAXSAT = "i-m";
  const std::string PruningOptions::LAYER_FREQUENCY = "i-f";
  const std::string PruningOptions::LAYER_MAXSAT_FREQUENCY = "i-mf";
  const std::vector<std::string> PruningOptions::get_all() {
    return {
        NONE,
        ALL_MAXSAT,
        LAYER_GREEDY,
        LAYER_MAXSAT,
        LAYER_FREQUENCY,
        LAYER_MAXSAT_FREQUENCY,
    };
  }
}  // namespace feature_generation
