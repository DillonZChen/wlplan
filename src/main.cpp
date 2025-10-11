#include "../include/data/dataset.hpp"
#include "../include/feature_generator/feature_generators/ccwl.hpp"
#include "../include/feature_generator/feature_generators/ccwla.hpp"
#include "../include/feature_generator/feature_generators/iwl.hpp"
#include "../include/feature_generator/feature_generators/kwl2.hpp"
#include "../include/feature_generator/feature_generators/lwl2.hpp"
#include "../include/feature_generator/feature_generators/niwl.hpp"
#include "../include/feature_generator/feature_generators/wl.hpp"
#include "../include/feature_generator/features.hpp"
#include "../include/feature_generator/pruning_options.hpp"
#include "../include/graph_generator/graph_generators/aoag.hpp"
#include "../include/graph_generator/graph_generators/iilg.hpp"
#include "../include/graph_generator/graph_generators/ilg.hpp"
#include "../include/graph_generator/graph_generators/nilg.hpp"
#include "../include/graph_generator/graph_generators/ploig.hpp"
#include "../include/planning/atom.hpp"
#include "../include/planning/domain.hpp"
#include "../include/planning/fluent.hpp"
#include "../include/planning/function.hpp"
#include "../include/planning/numeric_condition.hpp"
#include "../include/planning/numeric_expression.hpp"
#include "../include/planning/object.hpp"
#include "../include/planning/predicate.hpp"
#include "../include/planning/problem.hpp"
#include "../include/planning/schema.hpp"
#include "../include/serialise.hpp"
#include "../include/utils/exceptions.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/typing.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace py::literals;

PYBIND11_MODULE(_wlplan, m) {
  m.doc() = "WLPlan: WL Features for PDDL Planning";

  py::register_exception_translator([](std::exception_ptr p) {
    try {
      if (p)
        std::rethrow_exception(p);
    } catch (const NotImplementedError &e) {
      PyErr_SetString(PyExc_NotImplementedError, e.what());
    } catch (const NotSupportedError &e) {
      PyErr_SetString(PyExc_ValueError, e.what());
    }
  });

  //////////////////////////////////////////////////////////////////////////////
  // Planning
  //////////////////////////////////////////////////////////////////////////////
  auto planning_m = m.def_submodule("planning");

  /* Domain components */

  // Predicate
  py::class_<wlplan::planning::Predicate>(planning_m,
                                          "Predicate",
                                          R"(Parameters
----------
    name : str
        Predicate name.

    arity : int
        Predicate arity.
)")
      .def(py::init<const std::string &, int>(), "name"_a, "arity"_a)
      .def_readonly("name", &wlplan::planning::Predicate::name)
      .def_readonly("arity", &wlplan::planning::Predicate::arity)
      .def("__repr__", &::wlplan::planning::Predicate::to_string)
      .def("__eq__", &::wlplan::planning::Predicate::operator==)
      .def(py::pickle(&__getstate__<wlplan::planning::Predicate>,
                      &__setstate__<wlplan::planning::Predicate>));

  // Function
  py::class_<wlplan::planning::Function>(planning_m,
                                         "Function",
                                         R"(Parameters
----------
    name : str
        Function name.

    arity : int
        Function arity.
)")
      .def(py::init<const std::string &, int>(), "name"_a, "arity"_a)
      .def_readonly("name", &wlplan::planning::Function::name)
      .def_readonly("arity", &wlplan::planning::Function::arity)
      .def("__repr__", &::wlplan::planning::Function::to_string)
      .def("__eq__", &::wlplan::planning::Function::operator==)
      .def(py::pickle(&__getstate__<wlplan::planning::Function>,
                      &__setstate__<wlplan::planning::Function>));

  // Schema
  py::class_<wlplan::planning::Schema>(planning_m,
                                       "Schema",
                                       R"(Parameters
----------
    name : str
        Schema name.

    arity : int
        Schema arity.
)")
      .def(py::init<const std::string &, int>(), "name"_a, "arity"_a)
      .def_readonly("name", &wlplan::planning::Schema::name)
      .def_readonly("arity", &wlplan::planning::Schema::arity)
      .def("__repr__", &::wlplan::planning::Schema::to_string)
      .def("__eq__", &::wlplan::planning::Schema::operator==)
      .def(py::pickle(&__getstate__<wlplan::planning::Schema>,
                      &__setstate__<wlplan::planning::Schema>));

  // Domain
  py::class_<wlplan::planning::Domain>(planning_m,
                                       "Domain",
                                       R"(Parameters
----------
    name : str
        Domain name.

    predicates : list[Predicate]
        List of predicates.

    functions : list[Function], optional
        List of functions.

    schemata : list[Schema], optional
        List of schemata.

    constant_objects : list[Object], optional
        List of constant objects.
)")
      .def(py::init<const std::string &,
                    const std::vector<wlplan::planning::Predicate>,
                    const std::vector<wlplan::planning::Function>,
                    const std::vector<wlplan::planning::Schema>,
                    const std::vector<wlplan::planning::Object>>(),
           "name"_a,
           "predicates"_a,
           "functions"_a,
           "schemata"_a,
           "constant_objects"_a)
      .def_readonly("name", &wlplan::planning::Domain::name)
      .def_readonly("predicates", &wlplan::planning::Domain::predicates)
      .def_readonly("functions", &wlplan::planning::Domain::functions)
      .def_readonly("schemata", &wlplan::planning::Domain::schemata)
      .def_readonly("constant_objects", &wlplan::planning::Domain::constant_objects)
      .def("__repr__", &::wlplan::planning::Domain::to_string)
      .def("__eq__", &::wlplan::planning::Domain::operator==)
      .def(py::pickle(&__getstate__<wlplan::planning::Domain>,
                      &__setstate__<wlplan::planning::Domain>));

  /* Task components */

  // Object
  py::class_<wlplan::planning::Object>(
      planning_m,
      "Object",
      R"(Object is a type alias for a str. WLPlan does not exploit object types.
)");

  // Action
  py::class_<wlplan::planning::Action>(planning_m,
                                       "Action",
                                       R"(Parameters
----------
    schema : Schema
        Schema object.

    objects : list[Object]
        List of object names.
)")
      .def(py::init<const wlplan::planning::Schema &, const std::vector<std::string> &>(),
           "schema"_a,
           "objects"_a)
      .def_property_readonly("schema", &wlplan::planning::Action::get_schema)
      .def_property_readonly("objects", &wlplan::planning::Action::get_objects)
      .def("to_pddl", &wlplan::planning::Action::to_pddl)
      .def("__repr__", &::wlplan::planning::Action::to_string)
      .def("__eq__", &::wlplan::planning::Action::operator==)
      .def(py::pickle(&__getstate__<wlplan::planning::Action>,
                      &__setstate__<wlplan::planning::Action>));

  // Atom
  py::class_<wlplan::planning::Atom>(planning_m,
                                     "Atom",
                                     R"(Parameters
----------
    predicate : Predicate
        Predicate object.

    objects : list[Object]
        List of object names.
)")
      .def(py::init<const wlplan::planning::Predicate &, const std::vector<std::string> &>(),
           "predicate"_a,
           "objects"_a)
      .def_property_readonly("predicate", &wlplan::planning::Atom::get_predicate)
      .def_property_readonly("objects", &wlplan::planning::Atom::get_objects)
      .def("to_pddl", &wlplan::planning::Atom::to_pddl)
      .def("__repr__", &::wlplan::planning::Atom::to_string)
      .def("__eq__", &::wlplan::planning::Atom::operator==)
      .def(
          py::pickle(&__getstate__<wlplan::planning::Atom>, &__setstate__<wlplan::planning::Atom>));

  // Fluent
  py::class_<wlplan::planning::Fluent>(planning_m,
                                       "Fluent",
                                       R"(Parameters
----------
    function : Function
        Function object.

    objects : list[Object]
        List of object names.
)")
      .def(py::init<const wlplan::planning::Function &, const std::vector<std::string> &>(),
           "function"_a,
           "objects"_a)
      .def("__repr__", &::wlplan::planning::Fluent::to_string)
      .def("__eq__", &::wlplan::planning::Fluent::operator==)
      .def(py::pickle(&__getstate__<wlplan::planning::Fluent>,
                      &__setstate__<wlplan::planning::Fluent>));

  // OperatorType
  py::enum_<wlplan::planning::OperatorType>(planning_m, "OperatorType")
      .value("Plus", wlplan::planning::OperatorType::Plus)
      .value("Minus", wlplan::planning::OperatorType::Minus)
      .value("Multiply", wlplan::planning::OperatorType::Multiply)
      .value("Divide", wlplan::planning::OperatorType::Divide);

  // NumericExpression (FormulaExpression, ConstantExpression, FluentExpression)
  py::class_<wlplan::planning::NumericExpression,
             std::shared_ptr<wlplan::planning::NumericExpression>>(
      planning_m,
      "NumericExpression",
      R"(NumericExpression is an abstract class for numeric expressions.
)")
      .def("evaluate", &wlplan::planning::NumericExpression::evaluate, "values"_a)
      .def("get_fluent_ids", &wlplan::planning::NumericExpression::get_fluent_ids)
      .def("__repr__", &wlplan::planning::NumericExpression::to_string);

  // FormulaExpression
  py::class_<wlplan::planning::FormulaExpression,
             wlplan::planning::NumericExpression,
             std::shared_ptr<wlplan::planning::FormulaExpression>>(planning_m,
                                                                   "FormulaExpression",
                                                                   R"(Parameters
----------
    op_type : OperatorType
        Operator enum class.

    expr_a : NumericExpression
        Numeric expression.

    expr_b : NumericExpression
        Numeric expression.
)")
      .def(py::init<wlplan::planning::OperatorType,
                    std::shared_ptr<wlplan::planning::NumericExpression> &,
                    std::shared_ptr<wlplan::planning::NumericExpression> &>(),
           "op_type"_a,
           "expr_a"_a,
           "expr_b"_a);

  // ConstantExpression
  py::class_<wlplan::planning::ConstantExpression,
             wlplan::planning::NumericExpression,
             std::shared_ptr<wlplan::planning::ConstantExpression>>(planning_m,
                                                                    "ConstantExpression",
                                                                    R"(Parameters
----------
    value : float
        Numeric value.
)")
      .def(py::init<double>(), "value"_a);

  // FluentExpression
  py::class_<wlplan::planning::FluentExpression,
             wlplan::planning::NumericExpression,
             std::shared_ptr<wlplan::planning::FluentExpression>>(planning_m,
                                                                  "FluentExpression",
                                                                  R"(Parameters
----------
    id : int
        Fluent ID.

    fluent_name : str
        Fluent name.
)")
      .def(py::init<int, std::string>(), "id"_a, "fluent_name"_a);

  // ComparatorType
  py::enum_<wlplan::planning::ComparatorType>(planning_m, "ComparatorType")
      .value("GreaterThan", wlplan::planning::ComparatorType::GreaterThan)
      .value("GreaterThanOrEqual", wlplan::planning::ComparatorType::GreaterThanOrEqual)
      .value("Equal", wlplan::planning::ComparatorType::Equal);

  // NumericCondition
  py::class_<wlplan::planning::NumericCondition>(planning_m,
                                                 "NumericCondition",
                                                 R"(Parameters
----------
    comparator_type : ComparatorType
        Comparator enum class.

    expression : NumericExpression
        Numeric expression constituting the LHS of the condition :math:`\xi \unrhd 0`.
)")
      .def(py::init<wlplan::planning::ComparatorType,
                    std::shared_ptr<wlplan::planning::NumericExpression> &>(),
           "comparator_type"_a,
           "expression"_a)
      .def("evaluate_formula", &wlplan::planning::NumericCondition::evaluate_formula, "values"_a)
      .def("evaluate_error", &wlplan::planning::NumericCondition::evaluate_error, "values"_a)
      .def("evaluate_formula_and_error",
           &wlplan::planning::NumericCondition::evaluate_formula_and_error,
           "values"_a)
      .def(py::pickle(&__getstate__<wlplan::planning::NumericCondition>,
                      &__setstate__<wlplan::planning::NumericCondition>));

  // Problem
  py::class_<wlplan::planning::Problem>(planning_m,
                                        "Problem",
                                        R"(Parameters
----------
    domain : Domain
        Domain object.

    objects : list[Object]
        List of object names.

    statics: list[Atom], optional
        List of static atoms.

    fluents: list[Fluent], optional
        List of fluents.

    fluent_values: list[float], optional
        List of fluent values of the initial state of the problem.

    positive_goals : list[Atom]
        List of positive goal atoms.

    negative_goals : list[Atom]
        List of negative goal atoms.

    numeric_goals : list[NumericCondition], optional
        List of numeric goals.
)")
      .def(py::init<const wlplan::planning::Domain &,
                    const std::vector<wlplan::planning::Object> &,
                    const std::vector<wlplan::planning::Atom> &,
                    const std::vector<wlplan::planning::Fluent> &,
                    const std::vector<double> &,
                    const std::vector<wlplan::planning::Atom> &,
                    const std::vector<wlplan::planning::Atom> &,
                    const std::vector<wlplan::planning::NumericCondition> &>(),
           "domain"_a,
           "objects"_a,
           "statics"_a,
           "fluents"_a,
           "fluent_values"_a,
           "positive_goals"_a,
           "negative_goals"_a,
           "numeric_goals"_a)
      .def(py::init<const wlplan::planning::Domain &,
                    const std::vector<wlplan::planning::Object> &,
                    const std::vector<wlplan::planning::Fluent> &,
                    const std::vector<double> &,
                    const std::vector<wlplan::planning::Atom> &,
                    const std::vector<wlplan::planning::Atom> &,
                    const std::vector<wlplan::planning::NumericCondition> &>(),
           "domain"_a,
           "objects"_a,
           "fluents"_a,
           "fluent_values"_a,
           "positive_goals"_a,
           "negative_goals"_a,
           "numeric_goals"_a)
      .def(py::init<const wlplan::planning::Domain &,
                    const std::vector<wlplan::planning::Object> &,
                    const std::vector<wlplan::planning::Atom> &,
                    const std::vector<wlplan::planning::Atom> &>(),
           "domain"_a,
           "objects"_a,
           "positive_goals"_a,
           "negative_goals"_a)
      .def_property_readonly("domain", &wlplan::planning::Problem::get_domain)
      .def_property_readonly("objects", &wlplan::planning::Problem::get_problem_objects)
      .def_property_readonly("constant_objects", &wlplan::planning::Problem::get_constant_objects)
      .def_property_readonly("statics", &wlplan::planning::Problem::get_statics)
      .def_property_readonly("fluents", &wlplan::planning::Problem::get_fluents)
      .def_property_readonly("fluent_name_to_id", &wlplan::planning::Problem::get_fluent_name_to_id)
      .def_property_readonly("init_fluent_values", &wlplan::planning::Problem::get_fluent_values)
      .def_property_readonly("positive_goals", &wlplan::planning::Problem::get_positive_goals)
      .def_property_readonly("negative_goals", &wlplan::planning::Problem::get_negative_goals)
      .def_property_readonly("numeric_goals", &wlplan::planning::Problem::get_numeric_goals)
      .def("__repr__", &wlplan::planning::Problem::to_string)
      .def("__eq__", &wlplan::planning::Problem::operator==)
      .def(py::pickle(&__getstate__<wlplan::planning::Problem>,
                      &__setstate__<wlplan::planning::Problem>));

  // State
  py::class_<wlplan::planning::State>(planning_m,
                                      "State",
                                      R"(Parameters
----------
    atoms : list[Atom]
        List of atoms.

    values : list[float], optional
        List of values for fluents defined in the problem.
)")
      .def(py::init<const std::vector<wlplan::planning::Atom> &>(), "atoms"_a)
      .def(py::init<const std::vector<wlplan::planning::Atom> &, const std::vector<double> &>(),
           "atoms"_a,
           "values"_a)
      .def_property_readonly("atoms", &wlplan::planning::State::get_atoms)
      .def_readonly("values", &wlplan::planning::State::values)
      .def("__repr__", &::wlplan::planning::State::to_string)
      .def("__eq__", &::wlplan::planning::State::operator==)
      .def("__hash__", &::wlplan::planning::State::hash)
      .def(py::pickle(&__getstate__<wlplan::planning::State>,
                      &__setstate__<wlplan::planning::State>));

  //////////////////////////////////////////////////////////////////////////////
  // Data
  //////////////////////////////////////////////////////////////////////////////
  auto data_m = m.def_submodule("data");

  // DomainDataset
  py::class_<wlplan::data::DomainDataset>(data_m,
                                          "DomainDataset",
                                          R"(WLPlan dataset object.

Datasets contain a domain and a list of problem states.

Parameters
----------
    domain : Domain
        Domain object.

    data : list[ProblemDataset]
        List of problem states.
)")
      .def(py::init<const wlplan::planning::Domain &,
                    const std::vector<wlplan::data::ProblemDataset> &>(),
           "domain"_a,
           "data"_a)
      .def_readonly("domain", &wlplan::data::DomainDataset::domain)
      .def_readonly("data", &wlplan::data::DomainDataset::data)
      .def(py::pickle(&__getstate__<wlplan::data::DomainDataset>,
                      &__setstate__<wlplan::data::DomainDataset>));

  // ProblemDataset
  py::class_<wlplan::data::ProblemDataset>(data_m,
                                           "ProblemDataset",
                                           R"(Stores a problem and training states for the problem.

Upon initialisation, the problem and states are checked for consistency.

Parameters
----------
    problem : Problem
        Problem object.

    states : list[State]
        List of training states.

    actions : list[list[Action]], optional
        List of actions for each state.
)")
      .def(py::init<const wlplan::planning::Problem &,
                    const std::vector<wlplan::planning::State> &>(),
           "problem"_a,
           "states"_a)
      .def(py::init<const wlplan::planning::Problem &,
                    const std::vector<wlplan::planning::State> &,
                    const std::vector<wlplan::planning::Actions> &>(),
           "problem"_a,
           "states"_a,
           "actions"_a)
      .def_readonly("problem", &wlplan::data::ProblemDataset::problem)
      .def_readonly("states", &wlplan::data::ProblemDataset::states)
      .def_readonly("actions", &wlplan::data::ProblemDataset::actions)
      .def(py::pickle(&__getstate__<wlplan::data::ProblemDataset>,
                      &__setstate__<wlplan::data::ProblemDataset>));

  //////////////////////////////////////////////////////////////////////////////
  // Graph
  //////////////////////////////////////////////////////////////////////////////
  auto graph_generator_m = m.def_submodule("graph_generator");

  // Graph
  py::class_<wlplan::graph_generator::Graph, std::shared_ptr<wlplan::graph_generator::Graph>>(
      graph_generator_m,
      "Graph",
      R"(WLPlan graph object.

Graphs have integer node colours and edge labels.

Parameters
----------
    node_colours : list[int]
        List of node colours, where `node[i]` is the colour of node `i` indexed from 0.

    node_values : list[float], optional
        List of node values. Empty if not provided.

    node_names : list[str], optional
        List of node names, where `node_names[i]` is the name of node `i` indexed from 0.

    edges : list[list[tuple[int, int]]]
        List of labelled edges, where `edges[u] = [(r_1, v_1), ..., (r_k, v_k)]` represents edges from node `u` to nodes `v_1, ..., v_k` with labels `r_1, ..., r_k`, respectively. WLPlan graphs are directed so users must ensure that edges are undirected.

Attributes
----------
    node_colours : list[int]
        List of node colours.

    node_values : list[float]
        List of node values. Empty if not provided.

    edges : list[list[tuple[int, int]]]
        List of labelled edges.

Methods
-------
    get_node_name(u: int) -> str
        Get the name of node `u`.

    dump() -> None
        Print the graph representation.
)")
      .def(py::init<std::vector<int>,
                    std::vector<double>,
                    std::vector<std::string>,
                    std::vector<std::vector<std::pair<int, int>>>>(),
           "node_colours"_a,
           "node_values"_a,
           "node_names"_a,
           "edges"_a)
      .def(py::init<std::vector<int>,
                    std::vector<double>,
                    std::vector<std::vector<std::pair<int, int>>>>(),
           "node_colours"_a,
           "node_values"_a,
           "edges"_a)
      .def(py::init<std::vector<int>,
                    std::vector<std::string>,
                    std::vector<std::vector<std::pair<int, int>>>>(),
           "node_colours"_a,
           "node_names"_a,
           "edges"_a)
      .def(py::init<std::vector<int>, std::vector<std::vector<std::pair<int, int>>>>(),
           "node_colours"_a,
           "edges"_a)
      .def_readonly("node_colours", &wlplan::graph_generator::Graph::nodes)
      .def_readonly("node_values", &wlplan::graph_generator::Graph::node_values)
      .def_readonly("edges", &wlplan::graph_generator::Graph::edges)
      .def("get_node_name", &wlplan::graph_generator::Graph::get_node_name, "u"_a)
      .def("dump", &wlplan::graph_generator::Graph::dump)
      .def("__repr__", &::wlplan::graph_generator::Graph::to_string);

  // GraphGenerator
  py::class_<wlplan::graph_generator::GraphGenerator>(graph_generator_m, "GraphGenerator")
      .def("get_n_features", &wlplan::graph_generator::GraphGenerator::get_n_features)
      .def("get_n_relations", &wlplan::graph_generator::GraphGenerator::get_n_relations)
      .def("to_graphs", &wlplan::graph_generator::GraphGenerator::to_graphs, "dataset"_a)
      .def("set_problem", &wlplan::graph_generator::GraphGenerator::set_problem, "problem"_a)
      .def("to_graph",
           py::overload_cast<const wlplan::planning::State &>(
               &wlplan::graph_generator::GraphGenerator::to_graph),
           "state"_a)
      .def("to_graph",
           py::overload_cast<const wlplan::planning::State &, const wlplan::planning::Actions &>(
               &wlplan::graph_generator::GraphGenerator::to_graph),
           "state"_a,
           "actions"_a);

  // ILGGenerator
  py::class_<wlplan::graph_generator::ILGGenerator, wlplan::graph_generator::GraphGenerator>(
      graph_generator_m, "ILGGenerator")
      .def(py::init<wlplan::planning::Domain &, bool>(),
           "domain"_a,
           "differentiate_constant_objects"_a);

  // IILGGenerator
  py::class_<wlplan::graph_generator::IILGGenerator, wlplan::graph_generator::GraphGenerator>(
      graph_generator_m, "IILGGenerator")
      .def(py::init<wlplan::planning::Domain &>(), "domain"_a);

  // NILGGenerator
  py::class_<wlplan::graph_generator::NILGGenerator, wlplan::graph_generator::ILGGenerator>(
      graph_generator_m, "NILGGenerator")
      .def(py::init<wlplan::planning::Domain &, bool>(),
           "domain"_a,
           "differentiate_constant_objects"_a);

  // PLOIGGenerator
  py::class_<wlplan::graph_generator::PLOIGGenerator, wlplan::graph_generator::GraphGenerator>(
      graph_generator_m, "PLOIGGenerator")
      .def(py::init<wlplan::planning::Domain &, bool>(),
           "domain"_a,
           "differentiate_constant_objects"_a);

  // AOAGGenerator
  py::class_<wlplan::graph_generator::AOAGGenerator, wlplan::graph_generator::GraphGenerator>(
      graph_generator_m, "AOAGGenerator")
      .def(py::init<wlplan::planning::Domain &, bool>(),
           "domain"_a,
           "differentiate_constant_objects"_a);

  //////////////////////////////////////////////////////////////////////////////
  // Feature Generation
  //////////////////////////////////////////////////////////////////////////////
  auto feature_generator_m = m.def_submodule("feature_generator");

  // PruningOptions
  py::class_<wlplan::feature_generator::PruningOptions>(feature_generator_m, "PruningOptions")
      .def_readonly_static("NONE", &wlplan::feature_generator::PruningOptions::NONE)
      .def_static("get_all", &wlplan::feature_generator::PruningOptions::get_all);

  // Features
  py::class_<wlplan::feature_generator::Features>(feature_generator_m, "Features")
      .def("collect",
           py::overload_cast<const wlplan::data::DomainDataset>(
               &wlplan::feature_generator::Features::collect_from_dataset),
           "dataset"_a)
      .def("collect",
           py::overload_cast<const std::vector<wlplan::graph_generator::Graph> &>(
               &wlplan::feature_generator::Features::collect),
           "graphs"_a)
      .def("to_graphs", &wlplan::feature_generator::Features::to_graphs, "dataset"_a)
      .def("set_problem", &wlplan::feature_generator::Features::set_problem, "problem"_a)
      .def("get_string_representation",
           py::overload_cast<const wlplan::feature_generator::Embedding &>(
               &wlplan::feature_generator::Features::get_string_representation),
           "embedding"_a)
      .def("get_string_representation",
           py::overload_cast<const wlplan::planning::State &>(
               &wlplan::feature_generator::Features::get_string_representation),
           "state"_a)
      .def("embed",
           py::overload_cast<const wlplan::data::DomainDataset &>(
               &wlplan::feature_generator::Features::embed_dataset),
           "dataset"_a)
      .def("embed",
           py::overload_cast<const std::vector<wlplan::graph_generator::Graph> &>(
               &wlplan::feature_generator::Features::embed_graphs),
           "graphs"_a)
      .def("embed",
           py::overload_cast<const wlplan::graph_generator::Graph &>(
               &wlplan::feature_generator::Features::embed_graph),
           "graph"_a)
      .def("embed",
           py::overload_cast<const wlplan::planning::State &>(
               &wlplan::feature_generator::Features::embed_state),
           "state"_a)
      .def("get_n_features", &wlplan::feature_generator::Features::get_n_features)
      .def("get_n_colours", &wlplan::feature_generator::Features::get_n_colours)
      .def("get_seen_counts", &wlplan::feature_generator::Features::get_seen_counts)
      .def("get_unseen_counts", &wlplan::feature_generator::Features::get_unseen_counts)
      .def("get_layer_to_n_colours", &wlplan::feature_generator::Features::get_layer_to_n_colours)
      .def("get_layer_to_colours", &wlplan::feature_generator::Features::get_layer_to_colours)
      .def("get_colour_to_layer", &wlplan::feature_generator::Features::get_colour_to_layer)
      .def("print_init_colours", &wlplan::feature_generator::Features::print_init_colours)
      .def("get_feature_name", &wlplan::feature_generator::Features::get_feature_name)
      .def("get_graph_representation",
           &wlplan::feature_generator::Features::get_graph_representation)
      .def("get_iterations", &wlplan::feature_generator::Features::get_iterations)
      .def("get_pruning", &wlplan::feature_generator::Features::get_pruning)
      .def("set_pruning", &wlplan::feature_generator::Features::set_pruning, "pruning"_a)
      .def("set_weights", &wlplan::feature_generator::Features::set_weights, "weights"_a)
      .def("get_weights", &wlplan::feature_generator::Features::get_weights)
      .def("predict",
           py::overload_cast<const wlplan::graph_generator::Graph &>(
               &wlplan::feature_generator::Features::predict),
           "graph"_a)
      .def("predict",
           py::overload_cast<const wlplan::planning::State &>(
               &wlplan::feature_generator::Features::predict),
           "state"_a)
      .def("save",
           py::overload_cast<const std::string &>(&wlplan::feature_generator::Features::save),
           "filename"_a)
      .def("save",
           py::overload_cast<const std::string &, const std::vector<double> &>(
               &wlplan::feature_generator::Features::save),
           "filename"_a,
           "weights"_a);

  // WLFeatures
  py::class_<wlplan::feature_generator::WLFeatures, wlplan::feature_generator::Features>(
      feature_generator_m, "WLFeatures")
      .def(py::init<const std::string &>(), "filename"_a)
      .def(py::init<const std::string &, bool>(), "filename"_a, "quiet"_a)
      .def(py::init<wlplan::planning::Domain &, std::string, int, std::string, bool>(),
           "domain"_a,
           "graph_representation"_a,
           "iterations"_a,
           "pruning"_a,
           "multiset_hash"_a);

  // LWL2Features
  py::class_<wlplan::feature_generator::LWL2Features, wlplan::feature_generator::Features>(
      feature_generator_m, "LWL2Features")
      .def(py::init<const std::string &>(), "filename"_a)
      .def(py::init<const std::string &, bool>(), "filename"_a, "quiet"_a)
      .def(py::init<wlplan::planning::Domain &, std::string, int, std::string, bool>(),
           "domain"_a,
           "graph_representation"_a,
           "iterations"_a,
           "pruning"_a,
           "multiset_hash"_a);

  // KWL2Features
  py::class_<wlplan::feature_generator::KWL2Features, wlplan::feature_generator::Features>(
      feature_generator_m, "KWL2Features")
      .def(py::init<const std::string &>(), "filename"_a)
      .def(py::init<const std::string &, bool>(), "filename"_a, "quiet"_a)
      .def(py::init<wlplan::planning::Domain &, std::string, int, std::string, bool>(),
           "domain"_a,
           "graph_representation"_a,
           "iterations"_a,
           "pruning"_a,
           "multiset_hash"_a);

  // IWLFeatures
  py::class_<wlplan::feature_generator::IWLFeatures, wlplan::feature_generator::Features>(
      feature_generator_m, "IWLFeatures")
      .def(py::init<const std::string &>(), "filename"_a)
      .def(py::init<const std::string &, bool>(), "filename"_a, "quiet"_a)
      .def(py::init<wlplan::planning::Domain &, std::string, int, std::string, bool>(),
           "domain"_a,
           "graph_representation"_a,
           "iterations"_a,
           "pruning"_a,
           "multiset_hash"_a);

  // NIWLFeatures
  py::class_<wlplan::feature_generator::NIWLFeatures, wlplan::feature_generator::IWLFeatures>(
      feature_generator_m, "NIWLFeatures")
      .def(py::init<const std::string &>(), "filename"_a)
      .def(py::init<const std::string &, bool>(), "filename"_a, "quiet"_a)
      .def(py::init<wlplan::planning::Domain &, std::string, int, std::string, bool>(),
           "domain"_a,
           "graph_representation"_a,
           "iterations"_a,
           "pruning"_a,
           "multiset_hash"_a);

  // CCWLFeatures
  py::class_<wlplan::feature_generator::CCWLFeatures, wlplan::feature_generator::WLFeatures>(
      feature_generator_m, "CCWLFeatures")
      .def(py::init<const std::string &>(), "filename"_a)
      .def(py::init<const std::string &, bool>(), "filename"_a, "quiet"_a)
      .def(py::init<wlplan::planning::Domain &, std::string, int, std::string, bool>(),
           "domain"_a,
           "graph_representation"_a,
           "iterations"_a,
           "pruning"_a,
           "multiset_hash"_a);

  // CCWLaFeatures
  py::class_<wlplan::feature_generator::CCWLaFeatures, wlplan::feature_generator::WLFeatures>(
      feature_generator_m, "CCWLaFeatures")
      .def(py::init<const std::string &>(), "filename"_a)
      .def(py::init<const std::string &, bool>(), "filename"_a, "quiet"_a)
      .def(py::init<wlplan::planning::Domain &, std::string, int, std::string, bool>(),
           "domain"_a,
           "graph_representation"_a,
           "iterations"_a,
           "pruning"_a,
           "multiset_hash"_a);

//////////////////////////////////////////////////////////////////////////////
// Version
//////////////////////////////////////////////////////////////////////////////
#ifdef WLPLAN_VERSION
  m.attr("__version__") = MACRO_STRINGIFY(WLPLAN_VERSION);
#else
  m.attr("__version__") = "dev";
#endif
}
