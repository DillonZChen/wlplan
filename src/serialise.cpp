#include "../include/serialise.hpp"

#include "../include/planning/action.hpp"
#include "../include/planning/atom.hpp"
#include "../include/planning/domain.hpp"
#include "../include/planning/fluent.hpp"
#include "../include/planning/function.hpp"
#include "../include/planning/numeric_condition.hpp"
#include "../include/planning/numeric_expression.hpp"
#include "../include/planning/predicate.hpp"
#include "../include/planning/problem.hpp"
#include "../include/planning/schema.hpp"
#include "../include/planning/state.hpp"
#include "../include/utils/exceptions.hpp"

void check_state_validity(py::tuple t, std::string cls, size_t expected_size) {
  if (t.size() != expected_size) {
    throw std::runtime_error("Invalid state for " + cls + ": expected " +
                             std::to_string(expected_size) + " elements, got " +
                             std::to_string(t.size()));
  }
}

// Action
template <>
py::tuple __getstate__(const planning::Action &input) {
  return py::make_tuple(*(input.schema), input.objects);
}

template <>
planning::Action __setstate__(py::tuple t) {
  check_state_validity(t, "Action", 2);
  return planning::Action(t[0].cast<planning::Schema>(),
                          t[1].cast<std ::vector<planning::Object>>());
}

// Atom
template <>
py::tuple __getstate__(const planning::Atom &input) {
  return py::make_tuple(*(input.predicate), input.objects);
}

template <>
planning::Atom __setstate__(py::tuple t) {
  check_state_validity(t, "Atom", 2);
  return planning::Atom(t[0].cast<planning::Predicate>(),
                        t[1].cast<std::vector<planning::Object>>());
}

// Domain
template <>
py::tuple __getstate__(const planning::Domain &input) {
  return py::make_tuple(
      input.name, input.predicates, input.functions, input.schemata, input.constant_objects);
}

template <>
planning::Domain __setstate__(py::tuple t) {
  check_state_validity(t, "Domain", 5);
  return planning::Domain(t[0].cast<std::string>(),
                          t[1].cast<std::vector<planning::Predicate>>(),
                          t[2].cast<std::vector<planning::Function>>(),
                          t[3].cast<std::vector<planning::Schema>>(),
                          t[4].cast<std::vector<planning::Object>>());
}

// Fluent
template <>
py::tuple __getstate__(const planning::Fluent &input) {
  return py::make_tuple(*(input.function), input.objects);
}

template <>
planning::Fluent __setstate__(py::tuple t) {
  check_state_validity(t, "Fluent", 2);
  return planning::Fluent(t[0].cast<planning::Function>(),
                          t[1].cast<std::vector<planning::Object>>());
}

// Function
template <>
py::tuple __getstate__(const planning::Function &input) {
  return py::make_tuple(input.name, input.arity);
}

template <>
planning::Function __setstate__(py::tuple t) {
  check_state_validity(t, "Function", 2);
  return planning::Function(t[0].cast<std::string>(), t[1].cast<int>());
}

// NumericCondition
template <>
py::tuple __getstate__(const planning::NumericCondition &input) {
  throw NotImplementedException("NumericCondition serialisation");
}

template <>
planning::NumericCondition __setstate__(py::tuple t) {
  throw NotImplementedException("NumericCondition serialisation");
}

// Predicate
template <>
py::tuple __getstate__(const planning::Predicate &input) {
  return py::make_tuple(input.name, input.arity);
}

template <>
planning::Predicate __setstate__(py::tuple t) {
  check_state_validity(t, "Predicate", 2);
  return planning::Predicate(t[0].cast<std::string>(), t[1].cast<int>());
}

// Problem
template <>
py::tuple __getstate__(const planning::Problem &input) {
  return py::make_tuple(__getstate__<planning::Domain>(input.get_domain()),
                        input.get_problem_objects(),
                        input.get_statics(),
                        input.get_fluents(),
                        input.get_fluent_values(),
                        input.get_positive_goals(),
                        input.get_negative_goals(),
                        input.get_numeric_goals());
}

template <>
planning::Problem __setstate__(py::tuple t) {
  check_state_validity(t, "Problem", 8);
  return planning::Problem(__setstate__<planning::Domain>(t[0].cast<py::tuple>()),
                           t[1].cast<std::vector<planning::Object>>(),
                           t[2].cast<std::vector<planning::Atom>>(),
                           t[3].cast<std::vector<planning::Fluent>>(),
                           t[4].cast<std::vector<double>>(),
                           t[5].cast<std::vector<planning::Atom>>(),
                           t[6].cast<std::vector<planning::Atom>>(),
                           t[7].cast<std::vector<planning::NumericCondition>>());
}

// Schema
template <>
py::tuple __getstate__(const planning::Schema &input) {
  return py::make_tuple(input.name, input.arity);
}

template <>
planning::Schema __setstate__(py::tuple t) {
  check_state_validity(t, "Schema", 2);
  return planning::Schema(t[0].cast<std::string>(), t[1].cast<int>());
}

// State
template <>
py::tuple __getstate__(const planning::State &input) {
  return py::make_tuple(input.get_atoms(), input.get_values());
}

template <>
planning::State __setstate__(py::tuple t) {
  check_state_validity(t, "State", 2);
  return planning::State(t[0].cast<std::vector<planning::Atom>>(),
                         t[1].cast<std::vector<double>>());
}
