#include "../../include/feature_generator/feature_generator_loader.hpp"

#include "../../include/feature_generator/feature_generators/ccwl.hpp"
#include "../../include/feature_generator/feature_generators/ccwla.hpp"
#include "../../include/feature_generator/feature_generators/iwl.hpp"
#include "../../include/feature_generator/feature_generators/lwl2.hpp"
#include "../../include/feature_generator/feature_generators/niwl.hpp"
#include "../../include/feature_generator/feature_generators/wl.hpp"
#include "../../include/utils/nlohmann/json.hpp"

#include <fstream>
#include <sstream>

std::shared_ptr<wlplan::feature_generator::Features>
load_feature_generator(const std::string save_file) {
  std::ifstream i(save_file);
  json j;
  i >> j;
  std::cout << "Loading feature generator from file " << save_file << std::endl;
  std::string feature_name = j["feature_name"];
  std::shared_ptr<wlplan::feature_generator::Features> feature_generator;
  if (feature_name == "wl") {
    feature_generator = std::make_shared<wlplan::feature_generator::WLFeatures>(save_file);
  } else if (feature_name == "2-lwl") {
    feature_generator = std::make_shared<wlplan::feature_generator::LWL2Features>(save_file);
  } else if (feature_name == "ccwl") {
    feature_generator = std::make_shared<wlplan::feature_generator::CCWLFeatures>(save_file);
  } else if (feature_name == "ccwl-a") {
    feature_generator = std::make_shared<wlplan::feature_generator::CCWLaFeatures>(save_file);
  } else if (feature_name == "iwl") {
    feature_generator = std::make_shared<wlplan::feature_generator::IWLFeatures>(save_file);
  } else if (feature_name == "niwl") {
    feature_generator = std::make_shared<wlplan::feature_generator::NIWLFeatures>(save_file);
  } else {
    throw std::runtime_error("Feature name " + feature_name + " not recognised.");
  }
  std::cout << "Feature generator loaded!" << std::endl;
  return feature_generator;
}
