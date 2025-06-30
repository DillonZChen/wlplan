import logging
import random

import networkx as nx
import numpy as np
import pytest
from ipc23lt import get_domain_pddl
from ipc23lt import get_raw_dataset as get_ipc23lt_dataset
from neurips24 import get_raw_dataset as get_neurips24_dataset

from wlplan.feature_generator import init_feature_generator
from wlplan.graph_generator import ILGGenerator, NILGGenerator, from_networkx, to_networkx
from wlplan.planning import parse_domain

LOGGER = logging.getLogger(__name__)


def test_blocksworld_random_path_graph():
    """Test the custom graph feature using from_networkx"""
    random.seed(0)
    LOGGER.info("Getting raw dataset")
    domain, dataset, _ = get_ipc23lt_dataset(domain_name="blocksworld", keep_statics=False)
    LOGGER.info("Constructing feature generator")
    feature_generator = init_feature_generator(
        feature_algorithm="wl",
        domain=domain,
        graph_representation="custom",
        iterations=4,
        pruning="none",
    )
    graphs = []
    LOGGER.info("Converting to random path graphs")
    for _, states in dataset:
        for state in states:
            G = nx.Graph()
            for i, atom in enumerate(state.atoms):
                G.add_node(str(atom), colour=random.randint(0, 10))
                if i > 0:
                    G.add_edge(str(state.atoms[i - 1]), str(atom), label=random.randint(0, 10))
            G = from_networkx(G)
            graphs.append(G)
            G.dump()
    LOGGER.info("Collecting features")
    feature_generator.collect(graphs)
    LOGGER.info("Embedding")
    X = np.array(feature_generator.embed(graphs)).astype(float)
    n_features = feature_generator.get_n_features()
    assert X.shape[1] == n_features
    LOGGER.info(f"{n_features} features collected from random path graphs")


def test_ilg():
    """Test ILG generator does not crash"""
    domain, dataset, _ = get_ipc23lt_dataset(domain_name="blocksworld", keep_statics=False)
    ilg_generator = ILGGenerator(domain, differentiate_constant_objects=True)
    for problem, states in dataset:
        for state in states:
            ilg_generator.set_problem(problem)
            graph = ilg_generator.to_graph(state)
            nx_graph = to_networkx(graph)  # just checks to see no crash
            assert nx_graph is not None


def test_ilg_blocksworld_counts():
    domain_name = "blocksworld"
    domain_pddl = get_domain_pddl(domain_name=domain_name)
    domain = parse_domain(domain_path=domain_pddl, domain_name=domain_name, keep_statics=True)
    ilg_generator = ILGGenerator(domain, differentiate_constant_objects=True)
    assert (
        ilg_generator.get_n_features() == 1 + len(domain.predicates) * 5
    )  # also include neg goals
    assert ilg_generator.get_n_relations() == max(p.arity for p in domain.predicates)


def test_nilg():
    """Test NILG generator does not crash"""
    try:
        domain, dataset, _ = get_neurips24_dataset(domain_name="blocksworld", keep_statics=False)
    except NotImplementedError:
        pytest.skip()
    ilg_generator = NILGGenerator(domain, differentiate_constant_objects=True)
    for problem, states in dataset:
        for state in states:
            ilg_generator.set_problem(problem)
            graph = ilg_generator.to_graph(state)
            nx_graph = to_networkx(graph)  # just checks to see no crash
            assert nx_graph is not None


if __name__ == "__main__":
    # Manually test drawing
    import matplotlib.pyplot as plt

    for generator, dataset_getter, descrition in [
        (ILGGenerator, get_ipc23lt_dataset, "ILG"),
        # (NILGGenerator, get_neurips24_dataset, "NILG"),
    ]:
        domain, dataset, _ = dataset_getter(domain_name="blocksworld", keep_statics=False)
        ilg_generator = generator(domain)
        problem, states = dataset[0]
        state = states[0]
        ilg_generator.set_problem(problem)
        graph = ilg_generator.to_graph(state)
        nx_graph = to_networkx(graph)
        nx.draw(nx_graph, with_labels=True)  # pyvis would be better but 2 more lines of code
        plt.savefig(f"blocksworld_{descrition}.png")
