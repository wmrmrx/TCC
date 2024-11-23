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
            edges: List = []
            edge_config = {}
            for e in edges_list:
                edges.append( (e.u, e.v) )
                edge_config[ (e.u, e.v) ] = { "stroke_color": COLOR_ARRAY[e.color] } 
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

        def new_scene(text, edges, vertices, scene, wait: float = 1, add_edge: bool = False):
            new_scene = draw(text, edges, vertices)
            self.play(manim.Transform(scene, new_scene))
            scene = new_scene
            self.wait(wait)

        vertices = []
        edges = []

        new_scene("Graphs have $n = 8$ vertices and degree greater or equal than $\\frac{n}{2}$", edges, vertices, scene, wait = 2)
        new_scene("Step 1: We can always find a rainbow path of size $\\frac{n}{2}$", edges, vertices, scene, wait = 2)
        new_scene("Greedily try to expand a rainbow path from vertex 0", edges, vertices, scene)
        
        vertices = [0]
        new_scene("", edges, vertices, scene)

        for color in range(0, n // 2 + 1):
            u = vertices[-1]
            v = None
            edge = None
            for optv in range(n):
                if optv not in vertices and G.check_edge(u, optv, color) is not None:
                    v = optv
                    edge = G.get_edge(u, v, color)
                    break
            assert v is not None
            vertices.append(v)
            edges.append(edge)
            new_scene("", edges, vertices, scene, add_edge = True)

        new_scene("Now we increment", edges, vertices, scene)
        new_scene("", edges, vertices, scene)

        while len(edges) != n:
            result = None
            if len(edges) == len(vertices):
                cycle = Cycle(G, deepcopy(vertices), deepcopy(edges))
                result = increment(G, cycle)
            else:
                path = Path(G, deepcopy(vertices), deepcopy(edges))
                result = increment(G, path)

            new_scene("", result.edges, result.vertices, scene, add_edge = len(result.edges) > len(edges))
            vertices = result.vertices
            edges = result.edges
