import os
from typing import Optional

from _wlplan.feature_generation import (PruningOptions, _CCWLFeatures, _IWLFeatures, _KWL2Features,
                                        _LWL2Features, _NIWLFeatures, _WLFeatures)
from _wlplan.planning import Domain

__all__ = [
    "get_feature_generator",
    "get_available_feature_generators",
    "Features",
    "WLFeatures",
    "IWLFeatures",
    "NIWLFeatures",
    "LWL2Features",
    "KWL2Features",
    "CCWLFeatures",
]


def _get_feature_generators_dict():
    return {
        "wl": WLFeatures,
        "kwl2": KWL2Features,
        "lwl2": LWL2Features,
        "ccwl": CCWLFeatures,
        "iwl": IWLFeatures,
        "niwl": NIWLFeatures,
    }


def get_available_graph_choices():
    return [None, "custom", "ilg", "nilg"]


def get_available_pruning_methods():
    return [None] + PruningOptions.get_all()


def get_available_feature_generators():
    return set(_get_feature_generators_dict().keys())


def get_feature_generator(feature_algorithm: str, domain: Domain, **kwargs):
    """
    Returns a feature generator based on the specified feature algorithm.

    Args:
        feature_algorithm (str): The feature algorithm to use.
        domain (Domain): The domain object.
        **kwargs: Additional keyword arguments to pass to the feature generator.

    Returns:
        FeatureGenerator: The instantiated feature generator.

    Raises:
        ValueError: If the specified feature algorithm is unknown.
    """
    FGs = _get_feature_generators_dict()
    if feature_algorithm in FGs:
        return FGs[feature_algorithm](domain=domain, **kwargs)
    else:
        raise ValueError(f"Unknown feature algorithm: {feature_algorithm}")


class Features:
    """Feature Generator object.

    Derived classes support graphs with nodes with categorical and continuous attributes and edge labels.

    Parameters
    ----------
        domain : Domain

        graph_representation : str, default="ilg"
            The graph encoding of planning states used. If None, the user can only call class method of classes and not datasets and states.

        iterations : int, default=2
            The number of WL iterations to perform.

        pruning : str, default=None
            How to detect and prune duplicate features. If None, no pruning is done.

        multiset_hash : bool, default=False
            Choose to use either set or multiset to store neighbour colours.

    Methods
    -------
        collect(self, dataset: Dataset) -> None
            Collect training colours from dataset.

        collect(self, graphs: List[Graph]) -> None
            Collect training colours from graphs.

        set_problem(self, problem: Problem) -> None
            Set problem for graph generator if it exists. This should be called before calling `embed` on a state.

        embed(self, dataset: Dataset) -> list[int]
            Converts a dataset into a 2D matrix in the form of a list of lists. Throws an error if training colours have not been collected by calling `collect`.

        embed(self, graphs: List[Graph]) -> list[int]
            Converts a list of graphs into a 2D matrix in the form of a list of lists. Throws an error if training colours have not been collected by calling `collect`.

        embed(self, state: State) -> list[int]
            Converts a state into a 1D embeddings. Throws an error if training colours have not been collected by calling `collect`. An error may also occur if the state does not belong to the problem set by `set_problem`, or if `set_problem` is not called beforehand.

        set_weights(self, weights: list[float]) -> None
            Set the weights to predict heuristics directly with this class. The weights must be a list of floats, integers or a numpy array of floats. The length of the weights must be the same as the number of features collected.

        get_weights(self) -> list[float]
            Return stored weights. Raises an error if weights do not exist.

        predict(self, state: State) -> float
            Predict a heuristic value for a state. The state must be a list of atoms. The weights must be set with `set_weights` before calling this method.
    """

    def __init__(
        self,
        domain: Optional[Domain],
        graph_representation: Optional[str],
        iterations: int,
        pruning: Optional[str],
        multiset_hash: bool,
        base_class=None,  # Pass the base class dynamically
        **kwargs,
    ) -> None:
        # Check if we want to load from a file. See Features.load
        if "filename" in kwargs:
            base_class.__init__(self, filename=kwargs["filename"])
            return

        graph_choices = get_available_graph_choices()
        if graph_representation not in graph_choices:
            raise ValueError(f"graph_representation must be one of {graph_choices}")
        if graph_representation is None:
            graph_representation = "custom"

        prune_choices = get_available_pruning_methods()
        if pruning not in prune_choices:
            raise ValueError(f"pruning must be one of {prune_choices}")
        if pruning is None:
            pruning = "none"

        base_class.__init__(
            self,
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            pruning=pruning,
            multiset_hash=multiset_hash,
        )

    def save(self, filename: str) -> None:
        filename_dir = os.path.dirname(filename)
        if len(filename_dir) > 0 and not os.path.exists(filename_dir):
            os.makedirs(filename_dir)
        super().save(filename)

    @staticmethod
    def load(filename: str, cls) -> "Features":
        return cls(domain=None, filename=filename)


class WLFeatures(Features, _WLFeatures):
    def __init__(
        self,
        domain: Domain,
        graph_representation: Optional[str] = "ilg",
        iterations: int = 2,
        pruning: Optional[str] = None,
        multiset_hash: bool = True,
        **kwargs,
    ) -> None:
        super().__init__(
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            pruning=pruning,
            multiset_hash=multiset_hash,
            base_class=_WLFeatures,  # Pass the correct base class
            **kwargs,
        )

    @staticmethod
    def load(filename: str) -> "WLFeatures":
        return Features.load(filename, WLFeatures)


class IWLFeatures(Features, _IWLFeatures):
    def __init__(
        self,
        domain: Domain,
        graph_representation: Optional[str] = "ilg",
        iterations: int = 2,
        pruning: Optional[str] = None,
        multiset_hash: bool = True,
        **kwargs,
    ) -> None:
        super().__init__(
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            pruning=pruning,
            multiset_hash=multiset_hash,
            base_class=_IWLFeatures,  # Pass the correct base class
            **kwargs,
        )

    @staticmethod
    def load(filename: str) -> "IWLFeatures":
        return Features.load(filename, IWLFeatures)


class NIWLFeatures(Features, _NIWLFeatures):
    def __init__(
        self,
        domain: Domain,
        graph_representation: Optional[str] = "ilg",
        iterations: int = 2,
        pruning: Optional[str] = None,
        multiset_hash: bool = True,
        **kwargs,
    ) -> None:
        super().__init__(
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            pruning=pruning,
            multiset_hash=multiset_hash,
            base_class=_NIWLFeatures,  # Pass the correct base class
            **kwargs,
        )

    @staticmethod
    def load(filename: str) -> "NIWLFeatures":
        return Features.load(filename, NIWLFeatures)


class LWL2Features(Features, _LWL2Features):
    def __init__(
        self,
        domain: Domain,
        graph_representation: Optional[str] = "ilg",
        iterations: int = 2,
        pruning: Optional[str] = None,
        multiset_hash: bool = True,
        **kwargs,
    ) -> None:
        super().__init__(
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            pruning=pruning,
            multiset_hash=multiset_hash,
            base_class=_LWL2Features,  # Pass the correct base class
            **kwargs,
        )

    @staticmethod
    def load(filename: str) -> "LWL2Features":
        return Features.load(filename, LWL2Features)


class KWL2Features(Features, _KWL2Features):
    def __init__(
        self,
        domain: Domain,
        graph_representation: Optional[str] = "ilg",
        iterations: int = 2,
        pruning: Optional[str] = None,
        multiset_hash: bool = True,
        **kwargs,
    ) -> None:
        super().__init__(
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            pruning=pruning,
            multiset_hash=multiset_hash,
            base_class=_KWL2Features,  # Pass the correct base class
            **kwargs,
        )

    @staticmethod
    def load(filename: str) -> "KWL2Features":
        return Features.load(filename, KWL2Features)


class CCWLFeatures(Features, _CCWLFeatures):
    def __init__(
        self,
        domain: Domain,
        graph_representation: Optional[str] = "nilg",
        iterations: int = 2,
        pruning: Optional[str] = None,
        multiset_hash: bool = False,  # NeurIPS-24 experiments used sets
        **kwargs,
    ) -> None:
        if graph_representation != "nilg":
            raise ValueError("ccWL only supports the graph_representation=nilg")
        super().__init__(
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            pruning=pruning,
            multiset_hash=multiset_hash,
            base_class=_CCWLFeatures,  # Pass the correct base class
            **kwargs,
        )

    @staticmethod
    def load(filename: str) -> "CCWLFeatures":
        return Features.load(filename, CCWLFeatures)
