#ifndef FEATURE_GENERATOR_FEATURE_PRUNING_OPTIONS_HPP
#define FEATURE_GENERATOR_FEATURE_PRUNING_OPTIONS_HPP

#include <string>
#include <vector>

namespace feature_generator {
  class PruningOptions {
   public:
    static const std::string NONE;
    static const std::string ALL_MAXSAT;
    static const std::string LAYER_GREEDY;  // bfg2019
    static const std::string LAYER_MAXSAT;  // MaxSAT
    static const std::string LAYER_FREQUENCY;  // frequency count < 1% n_data
    static const std::string LAYER_MAXSAT_FREQUENCY;  // MaxSAT then frequency

    static const std::vector<std::string> get_all();
  };
}  // namespace feature_generator

#endif  // FEATURE_GENERATOR_FEATURE_PRUNING_OPTIONS_HPP
