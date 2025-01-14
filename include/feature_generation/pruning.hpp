#ifndef FEATURE_GENERATION_PRUNING_HPP
#define FEATURE_GENERATION_PRUNING_HPP

#include <string>
#include <vector>

namespace feature_generation {
  class PruningOptions {
   public:
    static const std::string NONE;
    static const std::string COLLAPSE_ALL;
    static const std::string COLLAPSE_LAYER;

    static const std::vector<std::string> get_all();
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_PRUNING_HPP
