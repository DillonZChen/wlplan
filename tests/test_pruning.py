import logging
from itertools import product

import pytest
from colours import DOMAINS, colours_test

from wlplan.feature_generation import PruningOptions

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name,pruning", product(DOMAINS, PruningOptions.get_all()))
def test_domain(domain_name, pruning):
    if pruning == PruningOptions.NONE:
        pytest.skip()
    colours_test(domain_name, 2, "wl", pruning)


@pytest.mark.parametrize("domain_name,pruning", product(DOMAINS, ["i-mf"]))
def test_expressive(domain_name, pruning):
    colours_test(domain_name, 2, "lwl2", pruning)
