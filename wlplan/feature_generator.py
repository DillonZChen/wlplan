import json
import os

from _wlplan.feature_generator import (
    CCWLaFeatures,
    CCWLFeatures,
    Features,
    IWLFeatures,
    KWL2Features,
    LWL2Features,
    NIWLFeatures,
    PruningOptions,
    WLFeatures,
)
from _wlplan.planning import Domain


__all__ = [
    "init_feature_generator",
    "get_available_feature_algorithms",
    "Features",
]

_FEATURE_ALGORITHMS = {
    "wl": WLFeatures,
    "kwl2": KWL2Features,
    "lwl2": LWL2Features,
    "iwl": IWLFeatures,
    "niwl": NIWLFeatures,
    "ccwl": CCWLFeatures,
    "ccwl-a": CCWLaFeatures,
}


def get_available_graph_choices() -> list[str]:
    return ["custom", "ilg", "nilg", "ploig", "aoag"]


def get_available_pruning_methods() -> list[str]:
    return PruningOptions.get_all()


def get_available_feature_algorithms() -> list[str]:
    return list(_FEATURE_ALGORITHMS.keys())


def init_feature_generator(
    feature_algorithm: str,
    domain: Domain,
    graph_representation: str = "ilg",
    iterations: int = 2,
    pruning: str = "none",
    multiset_hash: bool = False,
) -> Features:
    """
    Returns a feature generator based on the specified feature algorithm.

    Parameters
    ----------
        feature_algorithm : str
            The WL feature algorithm to use.

        domain : Domain
            The input domain.

        graph_representation : str, default="ilg"
            The graph encoding of planning states used. If `"custom"`, the user can only call class method of classes and not datasets and states.

        iterations : int, default=2
            The number of WL iterations to perform.

        pruning : str, default=None
            How to detect and prune duplicate features. If `"none"`, no pruning is done.

        multiset_hash : bool, default=False
            Choose to use either set or multiset to store neighbour colours.

    Returns
    -------
        FeatureGenerator: The instantiated feature generator.

    Raises
    ------
        ValueError: If a specified argument is unknown.
    """
    feature_algorithm_choices = get_available_feature_algorithms()
    if feature_algorithm in _FEATURE_ALGORITHMS:
        FeatureGenerator = _FEATURE_ALGORITHMS[feature_algorithm]
    else:
        raise ValueError(
            f"Unknown value {feature_algorithm=}. Must be from {feature_algorithm_choices}"
        )

    graph_choices = get_available_graph_choices()
    if graph_representation not in graph_choices:
        raise ValueError(f"Unknown value {graph_representation=}. Must be from {graph_choices}")

    prune_choices = get_available_pruning_methods()
    if pruning not in prune_choices:
        raise ValueError(f"Unknown value {pruning=}. Must be from {prune_choices}")

    if pruning.startswith("i-") and feature_algorithm not in {"wl", "lwl2"}:
        raise NotImplementedError(f"{pruning=} and {feature_algorithm=} are not compatible")

    return FeatureGenerator(
        domain=domain,
        graph_representation=graph_representation,
        iterations=iterations,
        pruning=pruning,
        multiset_hash=multiset_hash,
    )


def load_feature_generator(filename: str, quiet: bool = False) -> Features:
    """
    Load a feature generator from a file.

    Parameters
    ----------
        filename : str
            The file to load the feature generator from.

        quiet : bool, default=False
            If True, suppress model information logging

    Returns
    -------
        FeatureGenerator: The loaded feature generator.
    """
    if not os.path.exists(filename):
        raise FileNotFoundError(f"Model file not found: {filename}")

    with open(filename, "r") as f:
        data = json.load(f)
    feature_generator = data["feature_name"]

    if feature_generator not in _FEATURE_ALGORITHMS:
        raise ValueError(f"Unknown {feature_generator=} in {filename=}")

    return _FEATURE_ALGORITHMS[feature_generator](filename=filename, quiet=quiet)
