import logging
import os
import zipfile

import pymimir

import wlplan
from wlplan.data import Dataset, ProblemStates
from wlplan.planning import Predicate, State, parse_domain

LOGGER = logging.getLogger(__name__)
DOMAINS = {
    "blocksworld",
    "childsnack",
    "ferry",
    "miconic",
    "rovers",
    "satellite",
    "spanner",
    "transport",
}


def get_domain_benchmark_dir(domain_name: str):
    assert domain_name in DOMAINS

    file_dir = os.path.dirname(os.path.abspath(__file__))

    ret = f"{file_dir}/benchmarks/{domain_name}"
    if not os.path.exists(ret):
        zip_file_path = f"{file_dir}/benchmarks.zip"
        extract_dir = f"{file_dir}"

        with zipfile.ZipFile(zip_file_path, "r") as zip_ref:
            zip_ref.extractall(extract_dir)

    return ret


def get_domain_pddl(domain_name: str):
    assert domain_name in DOMAINS
    benchmark_dir = get_domain_benchmark_dir(domain_name)
    domain_pddl = f"{benchmark_dir}/domain.pddl"
    return domain_pddl


def get_problem_pddl(domain_name: str, problem_name: str):
    assert domain_name in DOMAINS
    benchmark_dir = get_domain_benchmark_dir(domain_name)
    problem_pddl = f"{benchmark_dir}/testing/p{problem_name}.pddl"
    assert os.path.exists(problem_pddl), problem_pddl
    return problem_pddl


def get_domain_problem(domain_name: str, problem_name: str):
    domain_pddl = get_domain_pddl(domain_name)
    problem_pddl = get_problem_pddl(domain_name, problem_name)

    domain = parse_domain(domain_pddl)
    problem = parse_domain(problem_pddl)
    return domain, problem


def get_predicates(mimir_domain: pymimir.Domain, keep_statics: bool):
    raise NotImplementedError


def get_raw_dataset(domain_name: str):
    raise NotImplementedError  # no nice numeric planning parser


def get_dataset(domain_name: str):
    domain, data, y = get_raw_dataset(domain_name)
    problem_states = []
    for problem, states in data:
        problem_states.append(ProblemStates(problem=problem, states=states))
    dataset = Dataset(domain=domain, data=problem_states)
    return domain, dataset, y
