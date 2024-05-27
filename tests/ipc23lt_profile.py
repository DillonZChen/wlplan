import logging
import time

from ipc23lt import get_dataset
from util import print_mat

from wlplan.feature_generation import WLFeatures

LOGGER = logging.getLogger(__name__)

REPEATS = 100


def helper(domain_name):
    data = {}

    configs = {
        "set": {"multiset_hash": False},
        "mset": {"multiset_hash": True},
    }

    for desc, config in configs.items():
        domain, dataset, _ = get_dataset(domain_name, keep_statics=False)
        feature_generator = WLFeatures(
            domain=domain,
            iterations=4,
            prune_features=None,
            multiset_hash=config["multiset_hash"],
        )
        feature_generator.collect(dataset)
        t = time.time()
        for _ in range(REPEATS):
            X = feature_generator.embed(dataset).astype(float)
        t = (time.time() - t) / REPEATS
        data[desc] = {
            "n_data": X.shape[0],
            "n_feat": X.shape[1],
            "time": f"{t:.4f}",
        }

    itr = next(iter(configs.keys()))
    mat = [["config"] + list(data[itr].keys())]
    for desc in configs:
        mat.append([desc] + list(data[desc].values()))
    print_mat(mat)


def test_blocksworld():
    helper("blocksworld")


def test_childsnack():
    helper("childsnack")


def test_ferry():
    helper("ferry")


def test_floortile():
    helper("floortile")


def test_miconic():
    helper("miconic")


def test_rovers():
    helper("rovers")


def test_satellite():
    helper("satellite")


def test_sokoban():
    helper("sokoban")


def test_spanner():
    helper("spanner")


def test_transport():
    helper("transport")
