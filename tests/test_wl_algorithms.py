import logging

import pytest
from colours import DOMAINS, colours_test

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name", DOMAINS)
@pytest.mark.parametrize("wl_algorithm", ["wl", "iwl", "niwl", "lwl2"])
def test_domain(domain_name: str, wl_algorithm: str):
    colours_test(domain_name=domain_name, iterations=2, feature_algorithm=wl_algorithm)
