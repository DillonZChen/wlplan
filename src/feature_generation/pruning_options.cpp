#include "../../include/feature_generation/pruning_options.hpp"

namespace feature_generation {
  const std::string PruningOptions::NONE = "none";
  const std::string PruningOptions::COLLAPSE_ALL = "collapse-all";
  const std::string PruningOptions::COLLAPSE_ALL_X = "collapse-all-x";
  const std::string PruningOptions::COLLAPSE_LAYER = "collapse-layer";
  const std::string PruningOptions::COLLAPSE_LAYER_X = "collapse-layer-x";
  const std::vector<std::string> PruningOptions::get_all() {
    return {
        NONE,
        COLLAPSE_ALL,
        COLLAPSE_ALL_X,
        COLLAPSE_LAYER,
        COLLAPSE_LAYER_X,
    };
  }
}  // namespace feature_generation
