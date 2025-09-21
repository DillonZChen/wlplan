#ifndef PLANNING_FLUENT_HPP
#define PLANNING_FLUENT_HPP

#include "function.hpp"
#include "object.hpp"

#include <memory>
#include <string>
#include <vector>

namespace wlplan {
  namespace planning {
    class Fluent {
     public:
      std::shared_ptr<Function> function;
      std::vector<Object> objects;

      Fluent(const Function &function, const std::vector<Object> &objects);

      std::string to_string() const;

      bool operator==(const Fluent &other) const {
        return *function == *other.function && objects == other.objects;
      }
    };

  }  // namespace planning
}  // namespace wlplan

#endif  // PLANNING_FLUENT_HPP
