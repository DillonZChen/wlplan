#include "../include/data/dataset.hpp"
#include "../include/feature_generation/feature_generators/ccwl.hpp"
#include "../include/feature_generation/feature_generators/ccwla.hpp"
#include "../include/feature_generation/feature_generators/iwl.hpp"
#include "../include/feature_generation/feature_generators/kwl2.hpp"
#include "../include/feature_generation/feature_generators/lwl2.hpp"
#include "../include/feature_generation/feature_generators/niwl.hpp"
#include "../include/feature_generation/feature_generators/wl.hpp"
#include "../include/feature_generation/features.hpp"
#include "../include/feature_generation/pruning_options.hpp"
#include "../include/graph/graph_generators/ilg.hpp"
#include "../include/graph/graph_generators/nilg.hpp"
#include "../include/graph/graph_generators/ploig.hpp"
#include "../include/planning/atom.hpp"
#include "../include/planning/domain.hpp"
#include "../include/planning/fluent.hpp"
#include "../include/planning/function.hpp"
#include "../include/planning/numeric_condition.hpp"
#include "../include/planning/numeric_expression.hpp"
#include "../include/planning/object.hpp"
#include "../include/planning/predicate.hpp"
#include "../include/planning/problem.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/typing.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace py::literals;

// clang-format off
PYBIND11_MODULE(_wlplan, m) {
m.doc() = "WLPlan: WL Features for PDDL Planning";



//////////////////////////////////////////////////////////////////////////////
// Planning
//////////////////////////////////////////////////////////////////////////////
auto planning_m = m.def_submodule("planning");

/* Domain components */

// Predicate
py::class_<planning::Predicate>(planning_m, "Predicate",
R"(Parameters
----------
    name : str
        Predicate name.

    arity : int
        Predicate arity.
)")
  .def(py::init<std::string &, int>(),
        "name"_a, "arity"_a)
  .def_readonly("name", &planning::Predicate::name)
  .def_readonly("arity", &planning::Predicate::arity)
  .def("__repr__", &::planning::Predicate::to_string)
  .def("__eq__", &::planning::Predicate::operator==);

// Function
py::class_<planning::Function>(planning_m, "Function",
R"(Parameters
----------
    name : str
        Predicate name.

    arity : int
        Predicate arity.
)")
  .def(py::init<std::string &, int>(),
        "name"_a, "arity"_a)
  .def_readonly("name", &planning::Function::name)
  .def_readonly("arity", &planning::Function::arity)
  .def("__repr__", &::planning::Function::to_string)
  .def("__eq__", &::planning::Function::operator==);

// Domain
py::class_<planning::Domain>(planning_m, "Domain",
R"(Parameters
----------
    name : str
        Domain name.

    predicates : list[Predicate]
        List of predicates.

    functions : list[Function], optional
        List of functions.

    constant_objects : list[Object], optional
        List of constant objects.
)")
  .def(py::init<std::string &, std::vector<planning::Predicate>, std::vector<planning::Function>, std::vector<planning::Predicate>, std::vector<planning::Object>>(),
        "name"_a, "predicates"_a, "functions"_a, "schemata"_a, "constant_objects"_a)
  .def_readonly("name", &planning::Domain::name)
  .def_readonly("predicates", &planning::Domain::predicates)
  .def_readonly("functions", &planning::Domain::functions)
  .def_readonly("constant_objects", &planning::Domain::constant_objects)
  .def("__repr__", &::planning::Domain::to_string)
  .def("__eq__", &::planning::Domain::operator==);

/* Task components */

// Object
py::class_<planning::Object>(planning_m, "Object",
R"(Object is a type alias for a str. WLPlan does not exploit object types.
)");

// Atom
py::class_<planning::Atom>(planning_m, "Atom",
R"(Parameters
----------
    predicate : Predicate
        Predicate object.

    objects : list[Object]
        List of object names.
)")
  .def(py::init<planning::Predicate &, std::vector<std::string> &>(),
        "predicate"_a, "objects"_a)
  .def("__repr__", &::planning::Atom::to_string)
  .def("__eq__", &::planning::Atom::operator==);

// Fluent
py::class_<planning::Fluent>(planning_m, "Fluent",
R"(Parameters
----------
    function : Function
        Function object.

    objects : list[Object]
        List of object names.
)")
  .def(py::init<planning::Function &, std::vector<std::string> &>(),
        "function"_a, "objects"_a)
  .def("__repr__", &::planning::Fluent::to_string)
  .def("__eq__", &::planning::Fluent::operator==);

// OperatorType
py::enum_<planning::OperatorType>(planning_m, "OperatorType")
  .value("Plus", planning::OperatorType::Plus)
  .value("Minus", planning::OperatorType::Minus)
  .value("Multiply", planning::OperatorType::Multiply)
  .value("Divide", planning::OperatorType::Divide);

// NumericExpression (FormulaExpression, ConstantExpression, FluentExpression)
py::class_<planning::NumericExpression, std::shared_ptr<planning::NumericExpression>>(planning_m, "NumericExpression",
R"(NumericExpression is an abstract class for numeric expressions.
)")
  .def("evaluate", &planning::NumericExpression::evaluate, "values"_a)
  .def("get_fluent_ids", &planning::NumericExpression::get_fluent_ids)
  .def("__repr__", &planning::NumericExpression::to_string);

// FormulaExpression
py::class_<planning::FormulaExpression, planning::NumericExpression, std::shared_ptr<planning::FormulaExpression>>(planning_m, "FormulaExpression",
R"(Parameters
----------
    op_type : OperatorType
        Operator enum class.

    expr_a : NumericExpression
        Numeric expression.

    expr_b : NumericExpression
        Numeric expression.
)")
  .def(py::init<planning::OperatorType, std::shared_ptr<planning::NumericExpression> &, std::shared_ptr<planning::NumericExpression> &>(),
        "op_type"_a, "expr_a"_a, "expr_b"_a);

// ConstantExpression
py::class_<planning::ConstantExpression, planning::NumericExpression, std::shared_ptr<planning::ConstantExpression>>(planning_m, "ConstantExpression",
R"(Parameters
----------
    value : float
        Numeric value.
)")
  .def(py::init<double>(),
        "value"_a);

// FluentExpression
py::class_<planning::FluentExpression, planning::NumericExpression, std::shared_ptr<planning::FluentExpression>>(planning_m, "FluentExpression",
R"(Parameters
----------
    id : int
        Fluent ID.

    fluent_name : str
        Fluent name.
)")
  .def(py::init<int, std::string>(),
        "id"_a, "fluent_name"_a);

// ComparatorType
py::enum_<planning::ComparatorType>(planning_m, "ComparatorType")
  .value("GreaterThan", planning::ComparatorType::GreaterThan)
  .value("GreaterThanOrEqual", planning::ComparatorType::GreaterThanOrEqual)
  .value("Equal", planning::ComparatorType::Equal);

// NumericCondition
py::class_<planning::NumericCondition>(planning_m, "NumericCondition",
R"(Parameters
----------
    comparator_type : ComparatorType
        Comparator enum class.

    expression : NumericExpression
        Numeric expression constituting the LHS of the condition :math:`\xi \unrhd 0`.
)")
  .def(py::init<planning::ComparatorType, std::shared_ptr<planning::NumericExpression> &>(),
        "comparator_type"_a, "expression"_a)
  .def("evaluate_formula", &planning::NumericCondition::evaluate_formula, "values"_a)
  .def("evaluate_error", &planning::NumericCondition::evaluate_error, "values"_a)
  .def("evaluate_formula_and_error", &planning::NumericCondition::evaluate_formula_and_error, "values"_a);

// Problem
py::class_<planning::Problem>(planning_m, "Problem",
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
  .def(py::init<planning::Domain &,
                std::vector<std::string> &,
                std::vector<planning::Atom> &,
                std::vector<planning::Fluent> &,
                std::vector<double> &,
                std::vector<planning::Atom> &,
                std::vector<planning::Atom> &,
                std::vector<planning::NumericCondition> &>(),
        "domain"_a,
        "objects"_a,
        "statics"_a,
        "fluents"_a,
        "fluent_values"_a,
        "positive_goals"_a,
        "negative_goals"_a,
        "numeric_goals"_a)
  .def(py::init<planning::Domain &,
                std::vector<std::string> &,
                std::vector<planning::Fluent> &,
                std::vector<double> &,
                std::vector<planning::Atom> &,
                std::vector<planning::Atom> &,
                std::vector<planning::NumericCondition> &>(),
        "domain"_a,
        "objects"_a,
        "fluents"_a,
        "fluent_values"_a,
        "positive_goals"_a,
        "negative_goals"_a,
        "numeric_goals"_a)
  .def(py::init<planning::Domain &,
                std::vector<std::string> &,
                std::vector<planning::Atom> &,
                std::vector<planning::Atom> &>(),
        "domain"_a,
        "objects"_a,
        "positive_goals"_a,
        "negative_goals"_a)
  .def_property_readonly("domain", &planning::Problem::get_domain)
  .def_property_readonly("objects", &planning::Problem::get_problem_objects)
  .def_property_readonly("constant_objects", &planning::Problem::get_constant_objects)
  .def_property_readonly("statics", &planning::Problem::get_statics)
  .def_property_readonly("fluents", &planning::Problem::get_fluents)
  .def_property_readonly("fluent_name_to_id", &planning::Problem::get_fluent_name_to_id)
  .def_property_readonly("init_fluent_values", &planning::Problem::get_fluent_values)
  .def_property_readonly("positive_goals", &planning::Problem::get_positive_goals)
  .def_property_readonly("negative_goals", &planning::Problem::get_negative_goals)
  .def_property_readonly("numeric_goals", &planning::Problem::get_numeric_goals)
  .def("dump", &planning::Problem::dump)
;

// State
py::class_<planning::State>(planning_m, "State",
R"(Parameters
----------
    atoms : list[Atom]
        List of atoms.

    values : list[float], optional
        List of values for fluents defined in the problem.
)")
  .def(py::init<std::vector<planning::Atom> &>(),
        "atoms"_a)
  .def(py::init<std::vector<planning::Atom> &, std::vector<double> &>(),
        "atoms"_a, "values"_a)
  .def_property_readonly("atoms", &planning::State::get_atoms)
  .def_readonly("values", &planning::State::values)
  .def("__repr__", &::planning::State::to_string)
  .def("__eq__", &::planning::State::operator==)
  .def("__hash__", &::planning::State::hash)
;



//////////////////////////////////////////////////////////////////////////////
// Data
//////////////////////////////////////////////////////////////////////////////
auto data_m = m.def_submodule("data");

// Dataset
py::class_<data::Dataset>(data_m, "Dataset",
R"(WLPlan dataset object.

Datasets contain a domain and a list of problem states.

Parameters
----------
    domain : Domain
        Domain object.

    data : list[ProblemStates]
        List of problem states.
)")
  .def(py::init<planning::Domain &, std::vector<data::ProblemStates> &>(),
        "domain"_a, "data"_a);

// ProblemStates
py::class_<data::ProblemStates>(data_m, "ProblemStates",
R"(Stores a problem and training states for the problem.

Upon initialisation, the problem and states are checked for consistency.

Parameters
----------
    problem : Problem
        Problem object.

    states : list[State]
        List of training states.
)")
  .def(py::init<planning::Problem &, std::vector<planning::State> &>(),
        "problem"_a, "states"_a)
  .def_readonly("problem", &data::ProblemStates::problem)
  .def_readonly("states", &data::ProblemStates::states);



//////////////////////////////////////////////////////////////////////////////
// Graph
//////////////////////////////////////////////////////////////////////////////
auto graph_m = m.def_submodule("graph");

// Graph
py::class_<graph::Graph, std::shared_ptr<graph::Graph>>(graph_m, "Graph",
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
  .def(py::init<std::vector<int>, std::vector<double>, std::vector<std::string>, std::vector<std::vector<std::pair<int, int>>>>(),
        "node_colours"_a, "node_values"_a, "node_names"_a, "edges"_a)
  .def(py::init<std::vector<int>, std::vector<double>, std::vector<std::vector<std::pair<int, int>>>>(),
        "node_colours"_a, "node_values"_a, "edges"_a)
  .def(py::init<std::vector<int>, std::vector<std::string>, std::vector<std::vector<std::pair<int, int>>>>(),
        "node_colours"_a, "node_names"_a, "edges"_a)
  .def(py::init<std::vector<int>, std::vector<std::vector<std::pair<int, int>>>>(),
        "node_colours"_a, "edges"_a)
  .def_readonly("node_colours", &graph::Graph::nodes)
  .def_readonly("node_values", &graph::Graph::node_values)
  .def_readonly("edges", &graph::Graph::edges)
  .def("get_node_name", &graph::Graph::get_node_name, "u"_a)
  .def("dump", &graph::Graph::dump)
  .def("__repr__", &::graph::Graph::to_string);


// ILGGenerator
py::class_<graph::ILGGenerator>(graph_m, "ILGGenerator")
  .def(py::init<planning::Domain &, bool>(),
        "domain"_a, "differentiate_constant_objects"_a)
  .def("set_problem", &graph::ILGGenerator::set_problem, "problem"_a)
  .def("to_graph", &graph::ILGGenerator::to_graph, "state"_a)
;

// NILGGenerator
py::class_<graph::NILGGenerator, graph::ILGGenerator>(graph_m, "NILGGenerator")
  .def(py::init<planning::Domain &, bool>(),
        "domain"_a, "differentiate_constant_objects"_a)
  .def("set_problem", &graph::NILGGenerator::set_problem, "problem"_a)
  .def("to_graph", &graph::NILGGenerator::to_graph, "state"_a)
;

// PLOIGGenerator
py::class_<graph::PLOIGGenerator>(graph_m, "PLOIGGenerator")
  .def(py::init<planning::Domain &, bool>(),
        "domain"_a, "differentiate_constant_objects"_a)
  .def("set_problem", &graph::PLOIGGenerator::set_problem, "problem"_a)
  .def("to_graph", &graph::PLOIGGenerator::to_graph, "state"_a)
;

//////////////////////////////////////////////////////////////////////////////
// Feature Generation
//////////////////////////////////////////////////////////////////////////////
auto feature_generation_m = m.def_submodule("feature_generation");

py::class_<feature_generation::PruningOptions>(feature_generation_m, "PruningOptions")
  .def_readonly_static("NONE", &feature_generation::PruningOptions::NONE)
  .def_static("get_all", &feature_generation::PruningOptions::get_all)
;

py::class_<feature_generation::Features>(feature_generation_m, "Features")
  .def("collect", py::overload_cast<const data::Dataset>(&feature_generation::Features::collect_from_dataset),
        "dataset"_a)
  .def("collect", py::overload_cast<const std::vector<graph::Graph> &>(&feature_generation::Features::collect),
        "graphs"_a)
  .def("convert_to_graphs", &feature_generation::Features::convert_to_graphs,
        "dataset"_a)
  .def("set_problem", &feature_generation::Features::set_problem,
        "problem"_a)
  .def("get_string_representation", py::overload_cast<const feature_generation::Embedding &>(&feature_generation::Features::get_string_representation),
        "embedding"_a)
  .def("get_string_representation", py::overload_cast<const planning::State &>(&feature_generation::Features::get_string_representation),
        "state"_a)
  .def("embed", py::overload_cast<const data::Dataset &>(&feature_generation::Features::embed_dataset),
        "dataset"_a)
  .def("embed", py::overload_cast<const std::vector<graph::Graph> &>(&feature_generation::Features::embed_graphs),
        "graphs"_a)
  .def("embed", py::overload_cast<const graph::Graph &>(&feature_generation::Features::embed_graph),
        "graph"_a)
  .def("embed", py::overload_cast<const planning::State &>(&feature_generation::Features::embed_state),
        "state"_a)
  .def("get_n_features", &feature_generation::Features::get_n_features)
  .def("get_seen_counts", &feature_generation::Features::get_seen_counts)
  .def("get_unseen_counts", &feature_generation::Features::get_unseen_counts)
  .def("get_layer_to_n_colours", &feature_generation::Features::get_layer_to_n_colours)
  .def("get_layer_to_colours", &feature_generation::Features::get_layer_to_colours)
  .def("get_colour_to_layer", &feature_generation::Features::get_colour_to_layer)
  .def("print_init_colours", &feature_generation::Features::print_init_colours)
  .def("get_feature_name", &feature_generation::Features::get_feature_name)
  .def("get_graph_representation", &feature_generation::Features::get_graph_representation)
  .def("get_iterations", &feature_generation::Features::get_iterations)
  .def("get_pruning", &feature_generation::Features::get_pruning)
  .def("set_pruning", &feature_generation::Features::set_pruning,
        "pruning"_a)
  .def("set_weights", &feature_generation::Features::set_weights,
        "weights"_a)
  .def("get_weights", &feature_generation::Features::get_weights)
  .def("predict", py::overload_cast<const graph::Graph &>(&feature_generation::Features::predict),
        "graph"_a)
  .def("predict", py::overload_cast<const planning::State &>(&feature_generation::Features::predict),
        "state"_a)
  .def("save", &feature_generation::Features::save)
;

py::class_<feature_generation::WLFeatures, feature_generation::Features>(feature_generation_m, "WLFeatures")
  .def(py::init<const std::string &>(),
        "filename"_a)
  .def(py::init<const std::string &, bool>(),
        "filename"_a, "quiet"_a)
  .def(py::init<planning::Domain &, std::string, int, std::string, bool>(),
        "domain"_a, "graph_representation"_a, "iterations"_a, "pruning"_a, "multiset_hash"_a)
;

py::class_<feature_generation::LWL2Features, feature_generation::Features>(feature_generation_m, "LWL2Features")
  .def(py::init<const std::string &>(),
        "filename"_a)
  .def(py::init<const std::string &, bool>(),
        "filename"_a, "quiet"_a)
  .def(py::init<planning::Domain &, std::string, int, std::string, bool>(),
        "domain"_a, "graph_representation"_a, "iterations"_a, "pruning"_a, "multiset_hash"_a)
;

py::class_<feature_generation::KWL2Features, feature_generation::Features>(feature_generation_m, "KWL2Features")
  .def(py::init<const std::string &>(),
        "filename"_a)
  .def(py::init<const std::string &, bool>(),
        "filename"_a, "quiet"_a)
  .def(py::init<planning::Domain &, std::string, int, std::string, bool>(),
        "domain"_a, "graph_representation"_a, "iterations"_a, "pruning"_a, "multiset_hash"_a)
;

py::class_<feature_generation::IWLFeatures, feature_generation::Features>(feature_generation_m, "IWLFeatures")
  .def(py::init<const std::string &>(),
        "filename"_a)
  .def(py::init<const std::string &, bool>(),
        "filename"_a, "quiet"_a)
  .def(py::init<planning::Domain &, std::string, int, std::string, bool>(),
        "domain"_a, "graph_representation"_a, "iterations"_a, "pruning"_a, "multiset_hash"_a)
;

py::class_<feature_generation::NIWLFeatures, feature_generation::IWLFeatures>(feature_generation_m, "NIWLFeatures")
  .def(py::init<const std::string &>(),
        "filename"_a)
  .def(py::init<const std::string &, bool>(),
        "filename"_a, "quiet"_a)
  .def(py::init<planning::Domain &, std::string, int, std::string, bool>(),
        "domain"_a, "graph_representation"_a, "iterations"_a, "pruning"_a, "multiset_hash"_a)
;

py::class_<feature_generation::CCWLFeatures, feature_generation::WLFeatures>(feature_generation_m, "CCWLFeatures")
  .def(py::init<const std::string &>(),
        "filename"_a)
  .def(py::init<const std::string &, bool>(),
        "filename"_a, "quiet"_a)
  .def(py::init<planning::Domain &, std::string, int, std::string, bool>(),
        "domain"_a, "graph_representation"_a, "iterations"_a, "pruning"_a, "multiset_hash"_a)
  .def("set_weights", &feature_generation::CCWLFeatures::set_weights,
        "weights"_a)
;

py::class_<feature_generation::CCWLaFeatures, feature_generation::WLFeatures>(feature_generation_m, "CCWLaFeatures")
  .def(py::init<const std::string &>(),
        "filename"_a)
  .def(py::init<const std::string &, bool>(),
        "filename"_a, "quiet"_a)
  .def(py::init<planning::Domain &, std::string, int, std::string, bool>(),
        "domain"_a, "graph_representation"_a, "iterations"_a, "pruning"_a, "multiset_hash"_a)
  .def("set_weights", &feature_generation::CCWLaFeatures::set_weights,
        "weights"_a)
;



//////////////////////////////////////////////////////////////////////////////
// Version
//////////////////////////////////////////////////////////////////////////////
#ifdef WLPLAN_VERSION
  m.attr("__version__") = MACRO_STRINGIFY(WLPLAN_VERSION);
#else
  m.attr("__version__") = "dev";
#endif
}
// clang-format on
