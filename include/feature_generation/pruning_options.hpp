#ifndef FEATURE_GENERATION_FEATURE_PRUNING_OPTIONS_HPP
#define FEATURE_GENERATION_FEATURE_PRUNING_OPTIONS_HPP

#include <string>
#include <vector>

namespace feature_generation {
  class PruningOptions {
   public:
    static const std::string NONE;
    static const std::string COLLAPSE_ALL;
    static const std::string COLLAPSE_ALL_X;
    static const std::string COLLAPSE_LAYER;
    static const std::string COLLAPSE_LAYER_X;
    static const std::string COLLAPSE_LAYER_Y;

    static const std::vector<std::string> get_all();
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_FEATURE_PRUNING_OPTIONS_HPP
