import math

import networkx as nx
import matplotlib.pyplot as plt
from networkx.algorithms.traversal.breadth_first_search import bfs_tree

# Helper for getting the amount of nodes in the network
def netsize(G):
    return len(G.nodes())

def visualize_libnet(libn):
    G = nx.DiGraph()
    nodes = libn.get_libs()
    edges = libn.get_deps()
    G.add_edges_from(edges)

    top_deps = sorted(G.nodes, key=G.in_degree, reverse=True)[:15]
    top_degrees = [G.in_degree(deg) for deg in top_deps]
    for i in range(15):
        print(f"{top_deps[i]}: {top_degrees[i]}")

    top_reqs = sorted(G.nodes, key=G.out_degree, reverse=True)[:15]
    t_trees = []
    for n in top_reqs:
        transitivity_tree = bfs_tree(G, n)
        t_trees.append(transitivity_tree)

    top_t_trees = sorted(t_trees, key=netsize, reverse=True)
    top_tree = top_t_trees[0]
    i = 0
    top_trans = ""
    for n in top_tree.nodes:
        print("Looking for root of top_tree...")
        top_trans = n
        if top_tree.in_degree(top_trans) == 0:
            print("Found")
            break

    print(f"Most transitive dependencies: {top_trans}")

    fixed_nodes = []
    labels = {}

    for e in G.edges:
        dep_deg = G.in_degree(e[1])
        G.edges[e]["color"] = dep_deg 
        G.edges[e]["width"] = math.log(dep_deg)/10

    for n in G.nodes:
        deg = G.in_degree(n)
        if deg > 30:
            fixed_nodes.append(n)
            labels[n] = n.split('/')[1]

        G.nodes[n]["size"] = deg
        G.nodes[n]["color"] = deg
   
    # Spring layout
    pos_init = nx.spring_layout(G, scale=1000, iterations=1, seed=10)
    pos = nx.spring_layout(G, pos=pos_init, fixed=fixed_nodes, seed=10)

    ### EDGES
    nx.draw_networkx_edges(
            G, pos,
            arrowstyle="->", 
            width=[width for width in nx.get_edge_attributes(G, "width").values()], 
            alpha=0.7,
            edge_color=[color for color in nx.get_edge_attributes(G, "color").values()],
            edge_cmap=plt.cm.inferno_r
    )

    ### NODES
    nx.draw_networkx_nodes(
            G, pos, 
            node_size=[size for size in nx.get_node_attributes(G, "size").values()],
            node_color=[color for color in nx.get_node_attributes(G, "color").values()],
            cmap=plt.cm.inferno_r
    )

    ### LABELS
    #nx.draw_networkx_labels(G, pos, labels, 
    #        verticalalignment="top",
    #        font_size=20
    #)

    mng = plt.get_current_fig_manager()
    mng.full_screen_toggle()
    plt.show()

