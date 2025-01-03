import sys
import itertools
import math
from increment import *
from copy import deepcopy
from graph import Vertex, Edge
import manim

manim.config["max_files_cached"] = 1000

COLOR_ARRAY = [manim.PURE_RED, manim.PURE_GREEN, manim.PURE_BLUE, 
               manim.RED, manim.GREEN, manim.BLUE,
               manim.PINK, manim.ORANGE]

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

class Draw(manim.Scene):
    def construct(self):
        instance = read_graph()
        G = instance
        n = G.n
        assert n == 8, f"Only n = 8 is supported, but n = {n}"

        def draw_graph(color, edges_list: List[Edge]):
            vertices: List = [i for i in range(n)]
            edges: List = [(i, j) for i in range(n) for j in range(i+1, n) if G.check_edge(i, j, color) is not None]
            edge_config = { edge: { "stroke_color": COLOR_ARRAY[color], "stroke_width": 1.2 } for edge in edges }
            for e in edges_list:
                (u, v) = (e.u, e.v) if e.u < e.v else (e.v, e.u)
                if e.color == color:
                    edge_config[(u, v)] = { "stroke_color": manim.GOLD, "stroke_width": 3 }
            return manim.Graph(
                    vertices = vertices, 
                    edges = edges,
                    layout = "circular",
                    layout_scale = 0.5,
                    vertex_config = { vertex: { "fill_color": COLOR_ARRAY[color], "radius": 0.07 } for vertex in vertices },
                    edge_config = edge_config
                )

        def draw_main_graph(edges_list: List[Edge], vertices):
            edges: List = [(i, j) for i in range(n) for j in range(i+1, n)]
            edge_config: Dict = { edge: { "stroke_width": 0.0 } for edge in edges }
            for e in edges_list:
                (u, v) = (e.u, e.v) if e.u < e.v else (e.v, e.u)
                edge_config[ (u, v) ] = { "stroke_color": COLOR_ARRAY[e.color] } 
            return manim.Graph(
                    vertices = [i for i in range(n)], 
                    edges = edges,
                    layout = "circular",
                    labels = True,
                    vertex_config = { vertex: {"fill_color": manim.GOLD} for vertex in vertices },
                    edge_config = edge_config
                    )

        def draw(text: str, edges: List[Edge], vertices: List[Vertex]):
            """Draws edges and highlights vertices"""
            graphs = manim.VGroup(*[
                    manim.VGroup(*[draw_graph(color, edges) for color in range(4 * block, 4 * block + 4)]).arrange(direction=manim.DOWN) for block in range(0, 2)
                ]).arrange()
            main_graph = draw_main_graph(edges, vertices)

            return manim.VGroup(
                manim.VGroup(graphs, main_graph).arrange(),
                manim.Tex(text)
            ).arrange(direction=manim.DOWN)

        scene = draw("", [], [])
        self.play(manim.Create(scene))

        def new_scene(text, edges, vertices, scene, wait: float = 1):
            new_scene = draw(text, edges, vertices)
            self.play(manim.Transform(scene, new_scene))
            scene = new_scene
            self.wait(wait)

        vertices = []
        edges = []

        new_scene("Graphs have $n = 8$ vertices and degree greater or equal than $\\frac{n}{2}$", edges, vertices, scene, wait = 5)

        new_scene("We start from a path of length $0$, starting at vertex $0$", edges, vertices, scene, wait = 5)
        
        vertices = [0]
        new_scene("", edges, vertices, scene)

        msg = Message()
        while len(edges) != n:
            result = None
            if len(edges) == len(vertices):
                cycle = Cycle(G, deepcopy(vertices), deepcopy(edges))
                result = increment(G, cycle, msg)
            else:
                path = Path(G, deepcopy(vertices), deepcopy(edges))
                result = increment(G, path, msg)
            new_scene(str(msg), result.edges, result.vertices, scene)
            new_scene("", result.edges, result.vertices, scene)
            vertices = result.vertices
            edges = result.edges
            msg = Message()

        new_scene("We have a Rainbow Hamiltonian Cycle.", edges, vertices, scene, wait = 5)

def run():
    instance = read_graph()
    G = instance
    n = G.n

    def print_cycle(cycle: Cycle):
        print(cycle.vertices)
        for e in cycle.edges:
            print(e.u, e.v, e.color)

    if n < 6:
        cycle = brute_force(G)
        print_cycle(cycle)
        return

    vertices = [0]
    edges = []
    while len(edges) != n:
        if len(edges) == len(vertices):
            cycle = Cycle(G, deepcopy(vertices), deepcopy(edges))
            result = increment(G, cycle)
        else:
            path = Path(G, deepcopy(vertices), deepcopy(edges))
            result = increment(G, path)
        vertices = result.vertices
        edges = result.edges

    assert len(edges) == n

if __name__ == "__main__":
    run()