import logging

import pytest
from ipc23lt import DOMAINS as IPC23LT_DOMAINS
from ipc23lt import get_domain_pddl as get_ipc23lt_domain_pddl
from neurips24 import DOMAINS as NEURIPS24_DOMAINS
from neurips24 import get_domain_pddl as get_neurips24_domain_pddl

from wlplan.planning import parse_domain

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name", sorted(IPC23LT_DOMAINS))
def test_profile(domain_name):
    domain_pddl = get_ipc23lt_domain_pddl(domain_name)
    domain = parse_domain(domain_pddl)
    LOGGER.info(domain)


@pytest.mark.parametrize("domain_name", sorted(NEURIPS24_DOMAINS))
def test_profile(domain_name):
    domain_pddl = get_neurips24_domain_pddl(domain_name)
    domain = parse_domain(domain_pddl)
    LOGGER.info(domain)
