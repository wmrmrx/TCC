import sys
import itertools
import math
from increment import *

def read_graph():
    n, m = map(int, sys.stdin.readline().split())
    G = Graph(n)

    for _ in range(m):
        u, v, color = map(int, sys.stdin.readline().split())
        u -= 1
        v -= 1
        color -= 1
        G.add_edge(u, v, color)

    for i in range(n):
        for vertex in range(n):
            if len(G.get_incident_edges(i, vertex)) < math.ceil(n / 2):
                raise ValueError("Input graph is not valid.")

    return G


def brute_force(instance: Graph) -> Cycle:
    G = instance
    n = G.n

    vertices = list(range(n))
    for perm_vertices in itertools.permutations(vertices):
        colors = list(range(n))
        for perm_colors in itertools.permutations(colors):
            edges = []
            for i in range(n):
                u = perm_vertices[i]
                v = perm_vertices[(i + 1) % n]
                color = perm_colors[i]
                edge = G.check_edge(u, v, color)
                if edge is None:
                    break
                edges.append(edge)
            if len(edges) == n:
                return Cycle(G, list(perm_vertices), edges)
    raise RuntimeError("No cycle found. Small test case.")


def print_cycle(cycle: Cycle):
    print(f"CYCLE SIZE: {cycle.size()}")
    for i in range(cycle.size()):
        u = cycle.vertices[i] + 1
        v = cycle.vertices[(i + 1) % cycle.size()] + 1
        edge = cycle.edges[i]
        assert edge.source == u - 1 and edge.target == v - 1
        print(f"{u} {v} {edge.color + 1}")


def print_path(path: Path):
    print(f"PATH SIZE: {len(path.edges)}")
    for i in range(len(path.vertices) - 1):
        u = path.vertices[i] + 1
        v = path.vertices[i + 1] + 1
        edge = path.edges[i]
        assert edge.source == u - 1 and edge.target == v - 1
        print(f"{edge.source} {edge.target} {edge.color}")


def print_object(obj):
    if isinstance(obj, Cycle):
        print_cycle(obj)
    elif isinstance(obj, Path):
        print_path(obj)


def main():
    instance = read_graph()
    G = instance

    n = G.n
    if n <= 6:
        cycle = brute_force(G)
        print_cycle(cycle)
    else:
        path = Path(G)
        path.push_back(0)
        obj = increment(G, path)
        while True:
            if isinstance(obj, Cycle):
                if obj.size() == n:
                    break
            obj = increment(G, obj)
        print_object(obj)


if __name__ == "__main__":
    main()
