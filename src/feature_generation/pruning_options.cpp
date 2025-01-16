#include "../../include/feature_generation/pruning_options.hpp"

namespace feature_generation {
  const std::string PruningOptions::NONE = "none";
  const std::string PruningOptions::COLLAPSE_ALL = "collapse-all";
  const std::string PruningOptions::COLLAPSE_LAYER = "collapse-layer";
  const std::vector<std::string> PruningOptions::get_all() {
    return {
        NONE,
        COLLAPSE_ALL,
        COLLAPSE_LAYER,
    };
  }
}  // namespace feature_generation
