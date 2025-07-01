import logging
import pickle
from typing import Any

import pytest
from ipc23lt import DOMAINS as IPC23LT_DOMAINS, get_domain_problem as get_ipc23lt_domain_problem
from neurips24 import (
    DOMAINS as NEURIPS24_DOMAINS,
    get_domain_problem as get_neurips24_domain_problem,
)


def save_then_load(input: Any):
    pkl_file = "tmp.pkl"
    try:
        with open(pkl_file, "wb") as f:
            pickle.dump(input, f)
        with open(pkl_file, "rb") as f:
            output = pickle.load(f)
    except NotImplementedError:
        pytest.skip()
    assert input == output, f"Serialisation failed for {type(input).__name__}\n{input=}\n{output=}"


@pytest.mark.parametrize("domain_name", sorted(IPC23LT_DOMAINS))
def test_classic(domain_name):
    domain, problem = get_ipc23lt_domain_problem(domain_name, problem_name="0_01")
    logging.info(f"{domain=}")
    logging.info(f"{problem=}")

    save_then_load(domain)
    save_then_load(problem)


@pytest.mark.parametrize("domain_name", sorted(NEURIPS24_DOMAINS))
def test_numeric(domain_name):
    domain, problem = get_neurips24_domain_problem(domain_name, problem_name="0_01")
    logging.info(f"{domain=}")
    logging.info(f"{problem=}")

    save_then_load(domain)
    save_then_load(problem)
