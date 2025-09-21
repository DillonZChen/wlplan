import logging

import pytest
from ipc23lt import DOMAINS as IPC23LT_DOMAINS, get_domain_pddl as get_ipc23lt_domain_pddl
from neurips24 import DOMAINS as NEURIPS24_DOMAINS, get_domain_pddl as get_neurips24_domain_pddl

from wlplan.planning import parse_domain


@pytest.mark.parametrize("domain_name", sorted(IPC23LT_DOMAINS))
def test_classic(domain_name):
    domain_pddl = get_ipc23lt_domain_pddl(domain_name)
    domain = parse_domain(domain_pddl)
    logging.info(domain)


@pytest.mark.parametrize("domain_name", sorted(NEURIPS24_DOMAINS))
def test_numeric(domain_name):
    domain_pddl = get_neurips24_domain_pddl(domain_name)
    domain = parse_domain(domain_pddl)
    logging.info(domain)
