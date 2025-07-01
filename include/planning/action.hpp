#ifndef PLANNING_ACTION_HPP
#define PLANNING_ACTION_HPP

#include "object.hpp"
#include "schema.hpp"

#include <memory>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/typing.h>
#include <string>
#include <vector>

namespace py = pybind11;

namespace planning {
  class Action {
   public:
    const std::shared_ptr<Schema> schema;
    const std::vector<Object> objects;

    Action(const Schema &schema, const std::vector<Object> &objects);

    Schema get_schema() const { return *schema; }
    std::vector<Object> get_objects() const { return objects; }

    std::string to_pddl() const;
    std::string to_string() const;

    bool operator==(const Action &other) const { return to_string() == other.to_string(); }
  };

}  // namespace planning

#endif  // PLANNING_ACTION_HPP
