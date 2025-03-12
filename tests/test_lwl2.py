import logging

import pytest
from colours import DOMAINS, colours_test

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name", DOMAINS)
def test_domain(domain_name):
    colours_test(domain_name, 2, "lwl2")
