import networkx as nx

from _wlplan.graph_generator import (
    Graph,
    GraphGenerator,
    ILGGenerator,
    NILGGenerator,
    PLOIGGenerator,
)
from _wlplan.planning import Domain

__all__ = [
    "init_graph_generator",
    "from_networkx",
    "to_networkx",
    "GraphGenerator",
]

_GRAPH_REPRESENTATIONS = {
    "ilg": ILGGenerator,
    "nilg": NILGGenerator,
    "ploig": PLOIGGenerator,
    # "aoag": AOAGGenerator,  # TODO
}


def get_available_graph_representations() -> list[str]:
    return list(_GRAPH_REPRESENTATIONS.keys())


def init_graph_generator(
    graph_representation: str,
    domain: Domain,
    differentiate_constant_objects: bool = True,
) -> GraphGenerator:
    """
    Returns a graph generator based on the specified graph representation.

    Parameters
    ----------
        graph_representation : str
            The graph encoding of planning states used.

        domain : Domain
            The input domain.

        differentiate_constant_objects : bool, default=True
            Choose to assign different colours to constant domain objects for supported graphs.

    Returns
    -------
        GraphGenerator: The instantiated graph generator.

    Raises
    ------
        ValueError: If a specified argument is unknown.
    """
    graph_representation_choices = get_available_graph_representations()
    if graph_representation in _GRAPH_REPRESENTATIONS:
        GraphGenerator = _GRAPH_REPRESENTATIONS[graph_representation]
    else:
        raise ValueError(
            f"Unknown value {graph_representation=}. Must be from {graph_representation_choices}"
        )

    return GraphGenerator(
        domain=domain,
        differentiate_constant_objects=differentiate_constant_objects,
    )


def from_networkx(graph: nx.Graph) -> Graph:
    """
    Converts a NetworkX graph to a WLPlan graph.

    Parameters
    ----------
        graph : nx.Graph
            Input NetworkX graph.

    Returns
    -------
        Graph : Output WLPlan graph.

    Raises
    ------
        ValueError: If node colours or edge labels are not specified in the graph attributes.
    """
    node_colours = []
    node_names = []
    name_to_idx = {}

    for i, (node, attr) in enumerate(graph.nodes(data=True)):
        if "colour" in attr:
            colour = attr["colour"]
        elif "color" in attr:
            colour = attr["color"]
        else:
            raise ValueError(f"Node colour not specified for node {node}")

        node_colours.append(colour)
        node_names.append(node)
        name_to_idx[node] = i

    edges = [[] for _ in range(len(node_names))]
    for u, v, attr in graph.edges(data=True):
        if "label" in attr:
            relation = attr["label"]
        elif "relation" in attr:
            relation = attr["relation"]
        else:
            raise ValueError(f"Edge label not specified for edge ({u}, {v})")

        edges[name_to_idx[u]].append((relation, name_to_idx[v]))

    wlplan_graph = Graph(node_colours, node_names, edges)

    return wlplan_graph


def to_networkx(graph: Graph) -> nx.Graph:
    """
    Converts a WLPlan graph to a NetworkX graph.

    Parameters
    ----------
        graph : nx.Graph
            Input WLPlan graph.

    Returns
    -------
        Graph : Output NetworkX graph.
    """
    G = nx.Graph()
    for u, colour in enumerate(graph.node_colours):
        node_name = graph.get_node_name(u)
        G.add_node(node_name, colour=colour)
    for u in range(len(graph.edges)):
        for r, v in graph.edges[u]:
            G.add_edge(graph.get_node_name(u), graph.get_node_name(v), relation=r)
    return G
