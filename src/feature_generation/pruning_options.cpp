#include "../../include/feature_generation/pruning_options.hpp"

namespace feature_generation {
  const std::string PruningOptions::NONE = "none";
  const std::string PruningOptions::COLLAPSE_ALL = "collapse-all";
  const std::string PruningOptions::COLLAPSE_LAYER = "collapse-layer";
  const std::string PruningOptions::COLLAPSE_LAYER_X = "collapse-layer-x";
  const std::vector<std::string> PruningOptions::get_all() {
    return {
        NONE,
        COLLAPSE_ALL,
        COLLAPSE_LAYER,
        COLLAPSE_LAYER_X,
    };
  }
  const bool PruningOptions::is_layer_pruning(const std::string &pruning_option) {
    return pruning_option == COLLAPSE_LAYER || pruning_option == COLLAPSE_LAYER_X;
  }
}  // namespace feature_generation
