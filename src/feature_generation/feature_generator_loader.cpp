#include "../../include/feature_generation/feature_generator_loader.hpp"

#include "../../include/feature_generation/ccwl_features.hpp"
#include "../../include/feature_generation/iwl_features.hpp"
#include "../../include/feature_generation/lwl2_features.hpp"
#include "../../include/feature_generation/niwl_features.hpp"
#include "../../include/feature_generation/wl_features.hpp"
#include "../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

std::shared_ptr<feature_generation::Features> load_feature_generator(const std::string save_file) {
  std::ifstream i(save_file);
  json j;
  i >> j;
  std::cout << "Loading feature generator from file " << save_file << std::endl;
  std::string feature_name = j["feature_name"];
  std::shared_ptr<feature_generation::Features> feature_generator;
  if (feature_name == "wl") {
    feature_generator = std::make_shared<feature_generation::WLFeatures>(save_file);
  } else if (feature_name == "2-lwl") {
    feature_generator = std::make_shared<feature_generation::LWL2Features>(save_file);
  } else if (feature_name == "ccwl") {
    feature_generator = std::make_shared<feature_generation::CCWLFeatures>(save_file);
  } else if (feature_name == "iwl") {
    feature_generator = std::make_shared<feature_generation::IWLFeatures>(save_file);
  } else if (feature_name == "niwl") {
    feature_generator = std::make_shared<feature_generation::NIWLFeatures>(save_file);
  } else {
    std::cout << "Feature name " << feature_name << " not recognised. Exiting." << std::endl;
    exit(-1);
  }
  std::cout << "Feature generator loaded!" << std::endl;
  return feature_generator;
}
