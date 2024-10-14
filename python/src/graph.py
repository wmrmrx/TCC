import sys
from collections import namedtuple
from typing import List, Tuple, Union

# Definindo Edge e Vertex como alias de int
Vertex = int

class Edge:
    def __init__(self, source: Vertex, target: Vertex, color: int):
        self.source = source
        self.target = target
        self.color = color

# Estrutura auxiliar para Graph
class BundledEdge:
    def __init__(self, color: int = sys.maxsize):
        self.color = color

class Graph:
    def __init__(self, n: int):
        self.n = n
        self.edges = []
        self.G = [{} for _ in range(n)]
        self.adjacency = [[[] for _ in range(n)] for _ in range(n)]  

    def add_edge(self, u: int, v: int, color: int):
        self.edges.append(Edge(u, v, color))
        self.G[color][(u, v)] = True
        self.G[color][(v, u)] = True
        self.adjacency[color][u].append(v)
        self.adjacency[color][v].append(u)

    def get_edges(self, color: int) -> List[Edge]:
        return [edge for edge in self.edges if edge.color == color]

    def get_edges(self, color: int, u: int) -> List[Edge]:
        return [edge for edge in self.edges if edge.color == color and (edge.source == u or edge.target == u)]
    
    def check_edge(self, u: int, v: int, color: int) -> tuple[bool, Edge]:
        exists = (u, v) in self.G[color]
        return exists, Edge(u, v, color)

class Path:
    def __init__(self, G: Graph, vertices: List[Vertex] = None, edges: List[Edge] = None):
        self.G = G
        n = G.n
        self.vertices = vertices if vertices else []
        self.edges = edges if edges else []
        if vertices and edges:
            assert len(vertices) == len(edges) + 1
            used_vertex = [0] * n
            used_colors = [0] * n
            for vertex in vertices:
                if used_vertex[vertex] > 0:
                    raise RuntimeError("Path has repeated vertices.")
                used_vertex[vertex] += 1
            for edge in edges:
                if used_colors[edge.color] > 0:
                    raise RuntimeError("Path has repeated colors.")
                used_colors[edge.color] += 1
            for i in range(len(vertices) - 1):
                valid = False
                if edges[i].source != vertices[i] and edges[i].target != vertices[(i + 1) % len(vertices)]:
                    valid = True
                if edges[i].source != vertices[(i + 1) % len(vertices)] and edges[i].target != vertices[i]:
                    valid = True
                if not valid:
                    raise RuntimeError("Cycle has invalid edges.")

    def size(self) -> int:
        return len(self.edges)

    def back(self) -> Vertex:
        return self.vertices[-1]

    def push_back(self, v: Vertex, e: Edge = None) -> bool:
        if len(self.vertices) > 0:
            assert e is not None, "Edge must be provided."
            self.edges.append(e)
        self.vertices.append(v)
        return True

    def pop_back(self):
        assert len(self.vertices) > 0, "Cannot pop from empty path."
        self.vertices.pop()
        self.edges.pop()

class Cycle:
    def __init__(self, G: Graph, vertices: List[Vertex] = None, edges: List[Edge] = None):
        self.G = G
        n = G.n
        self.vertices = vertices if vertices else []
        self.edges = edges if edges else []
        print("CYCLE", vertices, edges) 
        if vertices and edges:
            assert len(vertices) == len(edges)
            used_vertex = [0] * n
            used_colors = [0] * n
            for vertex in vertices:
                if used_vertex[vertex] > 0:
                    raise RuntimeError("Cycle has repeated vertices.")
                used_vertex[vertex] += 1
            for edge in edges:
                if used_colors[edge.color] > 0:
                    raise RuntimeError("Cycle has repeated colors.")
                used_colors[edge.color] += 1
            for i in range(len(vertices)):
                if edges[i].source != vertices[i]:
                    edges[i].source, edges[i].target = edges[i].target, edges[i].source
                if edges[i].source != vertices[i] or edges[i].target != vertices[(i + 1) % len(vertices)]:
                    raise RuntimeError("Cycle has invalid edges.")

    def size(self) -> int:
        return len(self.edges)
