import os
from typing import Optional

import pddl
import pddl.logic
import pddl.logic.functions
from _wlplan.planning import (Atom, ComparatorType, ConstantExpression, Domain, Fluent,
                              FluentExpression, FormulaExpression, Function, NumericCondition,
                              NumericExpression, OperatorType, Predicate, Problem, State)
from pddl.core import Domain as PDDLDomain

_PDDL_TO_WLPLAN_BINARY_OPS = {
    pddl.logic.functions.Plus: OperatorType.Plus,
    pddl.logic.functions.Minus: OperatorType.Minus,
    pddl.logic.functions.Times: OperatorType.Multiply,
    pddl.logic.functions.Divide: OperatorType.Divide,
}


def _get_predicates(pddl_domain: PDDLDomain, keep_statics: bool) -> dict[str, Predicate]:
    predicates = {}
    if keep_statics:
        for predicate in pddl_domain.predicates:
            name = predicate.name
            arity = predicate.arity
            predicates[name] = Predicate(name=name, arity=arity)
    else:
        for schema in pddl_domain.actions:
            if isinstance(schema.effect, pddl.logic.base.And):
                effects = schema.effect._operands
            else:
                effects = [schema.effect]
            for effect in effects:
                if isinstance(effect, pddl.logic.predicates.Predicate):
                    predicate = effect
                elif isinstance(effect, pddl.logic.base.Not):
                    predicate = effect._arg
                else:
                    continue
                name = predicate.name
                arity = predicate.arity
                predicate = Predicate(name=name, arity=arity)
                if name not in predicates:
                    predicates[name] = predicate
                else:
                    assert predicates[name] == predicate
    return predicates


def _get_functions(pddl_domain: PDDLDomain) -> dict[str, Function]:
    functions = {}
    for function in pddl_domain.functions:
        name = function.name
        arity = function.arity
        functions[name] = Function(name=name, arity=arity)
    return functions


def _convert_pddl_to_wlplan_expression(
    pddl_expression, fluent_to_id: dict[str, int]
) -> NumericExpression:
    if isinstance(pddl_expression, pddl.logic.functions.NumericFunction):
        toks = str(pddl_expression).replace(")", "").replace("(", "").split()
        fluent_name = toks[0] + "(" + ", ".join(toks[1:]) + ")"
        assert fluent_name in fluent_to_id
        expression = FluentExpression(fluent_to_id[fluent_name], fluent_name)
    elif isinstance(pddl_expression, pddl.logic.functions.NumericValue):
        value = float(pddl_expression.value)
        expression = ConstantExpression(value)
    else:
        assert isinstance(pddl_expression, tuple(_PDDL_TO_WLPLAN_BINARY_OPS.keys()))
        a = _convert_pddl_to_wlplan_expression(pddl_expression._operands[0], fluent_to_id)
        b = _convert_pddl_to_wlplan_expression(pddl_expression._operands[1], fluent_to_id)
        op_type = _PDDL_TO_WLPLAN_BINARY_OPS[type(pddl_expression)]
        expression = FormulaExpression(op_type, a, b)
    return expression


def parse_domain(
    domain_path: str,
    domain_name: Optional[str] = None,
    keep_statics: bool = True,
) -> Domain:
    """Parses a domain file and returns a Domain object.

    Args:
        domain_path (str): The path to the domain file.
        domain_name (str, optional): The name of the domain. If not provided, it will be extracted from the file. Defaults to None.
        keep_statics (bool, optional): Whether to keep static predicates in the domain, computed by taking the union of action effects. Defaults to True.
    """

    if not os.path.exists(domain_path):
        raise FileNotFoundError(f"Domain file {domain_path} does not exist.")

    with open(domain_path, "r") as f:
        domain_content = f.read()

    # Read domain name from file if not provided
    if domain_name is None:
        domain_name = domain_content.split("(domain ")[1]
        domain_name = domain_name.split(")")[0]

    # Parse domain with the pddl package
    pddl_domain = pddl.parse_domain(domain_path)

    # Get predicates
    predicates = _get_predicates(pddl_domain, keep_statics)
    predicates = sorted(list(predicates.values()), key=lambda x: repr(x))

    # Get functions
    functions = _get_functions(pddl_domain)
    functions = sorted(list(functions.values()), key=lambda x: repr(x))

    # Get constant objects (ignores types)
    constant_objects = set()
    if "(:constants" in domain_content:
        constant_objects_content = domain_content.split("(:constants")[1]
        constant_objects_content = constant_objects_content.split(")")[0]
        for line in constant_objects_content.split("\n"):
            line = line.strip()
            if len(line) == 0:
                continue
            lhs_rhs = line.split("-")  # type is on the rhs
            objects = lhs_rhs[0].split()
            constant_objects = constant_objects.union(set(objects))
    constant_objects = sorted(list(constant_objects))

    domain = Domain(
        name=domain_name,
        predicates=predicates,
        functions=functions,
        constant_objects=constant_objects,
    )
    return domain


def parse_problem(domain_path: str, problem_path: str, keep_statics: bool = True) -> Problem:
    """Parses a problem file and returns a Problem object.

    Args:
        domain_path (str): The path to the domain file.
        problem_path (str): The path to the problem file.
        keep_statics (bool, optional): Whether to keep static predicates in the parsed domain. Defaults to True.
    """

    if not os.path.exists(problem_path):
        raise FileNotFoundError(f"Problem file {problem_path} does not exist.")

    # Use the pddl package to help with parsing
    pddl_domain = pddl.parse_domain(domain_path)
    pddl_problem = pddl.parse_problem(problem_path)

    # Get domain information
    name_to_predicate = _get_predicates(pddl_domain, keep_statics)
    name_to_function = _get_functions(pddl_domain)

    # Get problem information
    objects = sorted(o.name for o in pddl_problem.objects)
    statics = []  # TODO: also requires checking keep_statics flag
    fluents = []
    fluent_values = []
    fluent_to_id = {}

    for formula in sorted(pddl_problem.init, key=lambda x: repr(x)):
        if not isinstance(formula, pddl.logic.functions.EqualTo):
            continue
        variable = formula._operands[0]
        value = float(formula._operands[1].value)
        function = name_to_function[variable.name]
        fluent_terms = [o.name for o in variable.terms]
        fluent_values.append(value)
        fluent = Fluent(function=function, objects=fluent_terms)
        fluent_to_id[str(fluent)] = len(fluents)
        fluents.append(fluent)

    # Get goal information
    wlplan_goals = {"positive": [], "negative": [], "numeric": []}

    goals = pddl_problem.goal
    if isinstance(goals, pddl.logic.base.And):  # multiple goals
        goals = goals._operands
    else:  # single goal
        goals = [goals]

    def handle_propositional_goal(goal):
        if isinstance(goal, pddl.logic.predicates.Predicate):
            a = goal
            goal_type = "positive"
        elif isinstance(goal, pddl.logic.base.Not):
            a = goal._arg
            goal_type = "negative"
        wlplan_atom = Atom(predicate=name_to_predicate[a.name], objects=[o.name for o in a.terms])
        wlplan_goals[goal_type].append(wlplan_atom)

    def handle_numeric_goal(goal):
        # Convert to a normal form of [expression \unrhd 0]
        lhs = goal._operands[0]
        rhs = goal._operands[1]
        if isinstance(goal, pddl.logic.functions.EqualTo):
            # lhs  = rhs ---> lhs - rhs =  0
            comparator_type = ComparatorType.Equal
            expression = pddl.logic.functions.Minus(lhs, rhs)
        elif isinstance(goal, pddl.logic.functions.LesserThan):
            # lhs <  rhs ---> rhs - lhs >  0
            comparator_type = ComparatorType.GreaterThan
            expression = pddl.logic.functions.Minus(rhs, lhs)
        elif isinstance(goal, pddl.logic.functions.LesserEqualThan):
            # lhs <= rhs ---> rhs - lhs >= 0
            comparator_type = ComparatorType.GreaterEqualThan
            expression = pddl.logic.functions.Minus(rhs, lhs)
        elif isinstance(goal, pddl.logic.functions.GreaterThan):
            # lhs >  rhs ---> lhs - rhs >  0
            comparator_type = ComparatorType.GreaterThan
            expression = pddl.logic.functions.Minus(lhs, rhs)
        elif isinstance(goal, pddl.logic.functions.GreaterEqualThan):
            # lhs >= rhs ---> lhs - rhs >= 0
            comparator_type = ComparatorType.GreaterEqualThan
            expression = pddl.logic.functions.Minus(lhs, rhs)
        expression = _convert_pddl_to_wlplan_expression(expression, fluent_to_id)
        numeric_goal = NumericCondition(comparator_type=comparator_type, expression=expression)
        wlplan_goals["numeric"].append(numeric_goal)

    for goal in goals:
        # propositional goals
        if isinstance(goal, (pddl.logic.predicates.Predicate, pddl.logic.base.Not)):
            handle_propositional_goal(goal)
        elif isinstance(
            goal,
            (
                pddl.logic.functions.EqualTo,
                pddl.logic.functions.LesserThan,
                pddl.logic.functions.LesserEqualThan,
                pddl.logic.functions.GreaterThan,
                pddl.logic.functions.GreaterEqualThan,
            ),
        ):
            handle_numeric_goal(goal)
        else:
            raise ValueError(f"Unknown goal {goal} with type {type(goal)}")

    problem = Problem(
        domain=parse_domain(domain_path),
        objects=objects,
        statics=statics,
        fluents=fluents,
        fluent_values=fluent_values,
        positive_goals=wlplan_goals["positive"],
        negative_goals=wlplan_goals["negative"],
        numeric_goals=wlplan_goals["numeric"],
    )
    return problem


if __name__ == "__main__":
    # Quick test
    import os

    CUR_DIR = os.path.dirname(os.path.abspath(__file__))

    benchmark_dir = f"{CUR_DIR}/../tests/neurips24"
    domain_path = f"{benchmark_dir}/benchmarks/childsnack/domain.pddl"
    problem_path = f"{benchmark_dir}/benchmarks/childsnack/testing/p0_30.pddl"
    if not os.path.exists(domain_path):
        import zipfile

        zip_file_path = f"{benchmark_dir}/benchmarks.zip"
        extract_dir = f"{benchmark_dir}"
        with zipfile.ZipFile(zip_file_path, "r") as zip_ref:
            zip_ref.extractall(extract_dir)
    keep_statics = False
    domain = parse_domain(domain_path, keep_statics=keep_statics)
    problem = parse_problem(domain_path, problem_path, keep_statics=keep_statics)
    print(domain)
    print(problem)
