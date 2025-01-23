#include "../../include/feature_generation/pruning_options.hpp"

namespace feature_generation {
  const std::string PruningOptions::NONE = "none";
  const std::string PruningOptions::COLLAPSE_ALL = "collapse-all";
  const std::string PruningOptions::COLLAPSE_ALL_X = "collapse-all-x";
  const std::string PruningOptions::COLLAPSE_LAYER = "collapse-layer";
  const std::string PruningOptions::COLLAPSE_LAYER_X = "collapse-layer-x";
  const std::string PruningOptions::COLLAPSE_LAYER_Y = "collapse-layer-y";
  const std::string PruningOptions::COLLAPSE_LAYER_F = "collapse-layer-f";
  const std::string PruningOptions::COLLAPSE_LAYER_YF = "collapse-layer-yf";
  const std::vector<std::string> PruningOptions::get_all() {
    return {
        NONE,
        // COLLAPSE_ALL,
        COLLAPSE_ALL_X,
        // COLLAPSE_LAYER,
        COLLAPSE_LAYER_X,
        COLLAPSE_LAYER_Y,
        COLLAPSE_LAYER_F,
        COLLAPSE_LAYER_YF,
    };
  }
}  // namespace feature_generation
