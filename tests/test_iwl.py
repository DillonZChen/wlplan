import logging

import pytest
from colours import FD_COLOURS, colours_test

from wlplan.feature_generation import IWLFeatures

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("domain_name", sorted(FD_COLOURS.keys()))
def test_domain(domain_name):
    colours_test(domain_name, 2, IWLFeatures)
