from pathlib import Path

import networkx as nx
import pandas as pd

from config import LEVELS
from interaction_network import add_edges_reaction_participants, add_edges_complex_components, add_nodes, save_graph, \
    read_graph
from lib.graph_database import get_query_result
from network_topology_queries import get_reaction_participants, get_complex_components, QUERIES_PARTICIPANTS, \
    fix_neo4j_values, QUERIES_COMPONENTS


def read_or_create_full_graph(level, sm=True, graphs_path="", v=False):
    """
    Creates the full interaction network (graph) and stores it in three .tsv files: entity nodes (vertices type 1),
    small molecule nodes (vertices type 2) and interactions (edges)
    Stores also two mapping files: genes_to_proteins.tsv and proteins_to_proteoforms.tsv
    :param level: One of the LEVEL enum
    :param sm: bool to add small molecules to the interaction network
    :param graphs_path: directory to store the files
    :param v: show extra messages to the console
    :return: networkx complete graph using all genes, proteins or proteoforms
    """

    graphs_path = Path(graphs_path)

    vertices_file = graphs_path / (level + "_vertices.tsv")
    edges_file = graphs_path / (level + "_interactions.tsv")
    small_molecules_file = graphs_path / (level + "_small_molecules_vertices.tsv")

    # Check if files exist
    if Path(edges_file).exists() and Path(vertices_file).exists() and (
            (sm and Path(small_molecules_file).exists()) or not sm):
        G = read_graph(graphs_path.as_posix(), "", level)
    else:
        print("Creating graph")
        G = nx.Graph()
        G.graph['num_' + level] = 0
        G.graph['num_small_molecules'] = 0

        participants = get_reaction_participants(level, True)
        components = get_complex_components(level, True)

        if sm:
            sm_participants = get_query_result(QUERIES_PARTICIPANTS['sm'])
            sm_participants = fix_neo4j_values(sm_participants, level)
            participants = pd.concat([sm_participants, participants])

            sm_components = get_query_result(QUERIES_COMPONENTS['sm'])
            sm_components = fix_neo4j_values(sm_components, level)
            components = pd.concat([sm_components, components])

        records = pd.concat([participants, components])
        add_nodes(G, records, level)

        add_edges_reaction_participants(G, participants)
        add_edges_complex_components(G, components)

        save_graph(G, level, graphs_path)

    G.graph["level"] = level
    G.graph["sm"] = sm

    if v:
        print(f"Graph edges: {G.size()}")
        print(f"Graph nodes: {len(G.nodes)}")
        print(f"Graph {level} nodes: {G.graph['num_' + level]}")
        print(f"Graph small molecule nodes: {G.graph['num_small_molecules']}")
    return G


if __name__ == '__main__':
    graphs_path = "../../resources/Reactome/"
    graphs = {l: read_or_create_full_graph(l, True, graphs_path) for l in LEVELS}
    for l in LEVELS:
        print(len(graphs[l]))
    # graphs_no_sm = [read_or_create_full_graph(level, False, graphs_path, v=False) for level in LEVELS]

    print("Finished!!")
