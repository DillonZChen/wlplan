import logging
from itertools import product

import pytest
from colours import DOMAINS, colours_test

from wlplan.feature_generation import PruningOptions, WLFeatures

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name,pruning", product(DOMAINS, PruningOptions.get_all()))
def test_domain(domain_name, pruning):
    if pruning == PruningOptions.NONE:
        pytest.skip()
    colours_test(domain_name, 4, WLFeatures, pruning)
