from typing import Optional, Union

import numpy as np

from _wlplan.data import Dataset
from _wlplan.feature_generation import _WLFeatures
from _wlplan.graph import Graph
from _wlplan.planning import Domain, State


class WLFeatures(_WLFeatures):
    """WL Feature Generator object.

    The implementation supports graphs with node colours and edge labels.

    Parameters
    ----------
        domain : Domain

        graph_representation : "ilg" or None, default="ilg"
            The graph encoding of planning states used. If None, the user can
            only call class method of classes and not datasets and states.

        iterations : int, default=2
            The number of WL iterations to perform.

        prune_features : "collapse", "collapse_by_layer" or None, default="collapse"
            How to detect and prune duplicate features. If None, no pruning is done.

        multiset_hash : bool, default=False
            Choose to use either set or multiset to store neighbour colours.

    Methods
    -------
        collect(dataset: Dataset) -> None
            Collect training colours from dataset.

        collect(graphs: List[Graph]) -> None
            Collect training colours from graphs.

        set_problem(problem: Problem) -> None
            Set problem for graph generator if it exists. This should be called before calling `embed` on a state.

        embed(dataset: Dataset) -> np.ndarray[np.int32]
            Converts a dataset into a numpy feature matrix. Throws an error if training colours have not been collected by calling `collect`.

        embed(graphs: List[Graph]) -> np.ndarray[np.int32]
            Converts a list of graphs into a numpy feature matrix. Throws an error if training colours have not been collected by calling `collect`.

        embed(state: List[Atom]) -> np.ndarray[np.int32]
            Converts a state into a numpy feature vector. Throws an error if training colours have not been collected by calling `collect`. An error may also occur if the state does not belong to the problem set by `set_problem`, or if `set_problem` is not called beforehand.
    """

    def __init__(
        self,
        domain: Domain,
        graph_representation: Optional[str] = "ilg",
        iterations: int = 2,
        prune_features: Optional[str] = "collapse",
        multiset_hash: bool = False,
    ) -> None:
        choices = [None, "ilg"]
        if graph_representation not in choices:
            raise ValueError(f"graph_representation must be one of {choices}")
        if graph_representation is None:
            graph_representation = "none"

        choices = [None, "collapse", "collapse_by_layer"]
        if prune_features not in choices:
            raise ValueError(f"prune_features must be one of {choices}")
        if prune_features is None:
            prune_features = "none"

        super().__init__(
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            prune_features=prune_features,
            multiset_hash=multiset_hash,
        )
