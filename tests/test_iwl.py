import logging

import pytest
from colours import DOMAINS, colours_test

from wlplan.feature_generation import IWLFeatures

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name", DOMAINS)
def test_domain(domain_name):
    colours_test(domain_name, 2, IWLFeatures)
