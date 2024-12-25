from graph import *
from math import ceil
from typing import Union, Optional
from enum import Enum
import pytest

class Case(Enum):
    SMALL_PATH = "Path length is smaller than $\\lceil \\frac{n}{2} \\rceil$. Greedily expand the path."
    UNDEFINED = "Undefined case."

    def __str__(self):
        return self.value 

class Message:
    case: Union[Case, str]

    def __init__(self):
        self.case = Case.UNDEFINED

    def __str__(self):
        return str(self.case)

def increment(G: Graph, path_or_cycle: Union[Path, Cycle], msg: Optional[Message] = None) -> Union[Path, Cycle]:
    if isinstance(path_or_cycle, Path):
        return handle_path(G, path_or_cycle, msg)
    elif isinstance(path_or_cycle, Cycle):
        return handle_cycle(G, path_or_cycle, msg)

def handle_path(G: Graph, path: Path, msg: Optional[Message] = None):
    n = G.n

    if len(path.vertices) == 0:
        raise RuntimeError("Path is empty.")

    colors_in_path = [False] * n
    vertices_in_path = [False] * n

    for edge in path.edges:
        colors_in_path[edge.color] = True

    for vertex in path.vertices:
        vertices_in_path[vertex] = True

    if path.size() < ceil(n / 2):
        msg and setattr(msg, 'case', Case.SMALL_PATH)
        vertices = path.vertices
        edges = path.edges

        color_outside_path = next(i for i in range(n) if not colors_in_path[i])

        for i in range(n):
            if not vertices_in_path[i]:
                edge = G.check_edge(path.back(), i, color_outside_path)
                if edge is not None:
                    return Path(G, vertices + [i], edges + [edge])
        raise RuntimeError("Should not reach here. Small path should return a new path.")
    else:
        cx = path.edges[-1].color
        cy = next(i for i in range(n) if not colors_in_path[i])
        path.pop_back()
        x, y = path.vertices[0], path.vertices[-1]

        for c in [cx, cy]:
            edge = G.check_edge(x, y, c)
            if edge is not None:
                vertices = path.vertices
                edges = path.edges + [edge]
                msg and setattr(msg, 'case', "Pop the last edge and find a cycle")
                return Cycle(G, vertices, edges)

        for [c1, c2] in [[cx, cy], [cy, cx]]:
            for i in range(n):
                if not vertices_in_path[i]:
                    edgeX = G.check_edge(x, i, c1)
                    edgeY = G.check_edge(y, i, c2)
                    if (edgeX is not None) and (edgeY is not None):
                        vertices = path.vertices + [i]
                        edges = path.edges + [edgeY, edgeX]
                        msg and setattr(msg, 'case', "Pop the last edge and find a cycle with a new vertex")
                        return Cycle(G, vertices, edges)

            for i in range(1, len(path.vertices) - 1):
                u, v = path.vertices[i], path.vertices[i + 1]
                edgeX = G.check_edge(x, v, c1)
                edgeY = G.check_edge(u, y, c2)

                if (edgeX is not None) and (edgeY is not None):
                    vertices = path.vertices[:i + 1] + [y] + path.vertices[i + 1:-1][::-1]
                    edges = path.edges[:i] + [edgeY] + path.edges[i + 1:][::-1] + [edgeX]
                    msg and setattr(msg, 'case', "Pop the last edge and find a cycle with the crossing trick")
                    return Cycle(G, vertices, edges)

    raise RuntimeError("Should not reach here. Did not find crossing.")

def handle_cycle(G: Graph, cycle: Cycle, msg: Optional[Message] = None):
    global increment_case
    n = G.n
    cycle_size = cycle.size()

    if cycle_size < 3:
        raise RuntimeError("Every cycle must have at least 3 vertices.")

    colors_in_cycle = [False] * n
    vertices_in_cycle = [False] * n

    for edge in cycle.edges:
        colors_in_cycle[edge.color] = True

    miss1 = next(i for i, c in enumerate(colors_in_cycle) if not c)
    miss2 = next(i for i in range(n - 1, -1, -1) if not colors_in_cycle[i])

    assert miss1 < miss2 or cycle_size == n - 1

    for vertex in cycle.vertices:
        vertices_in_cycle[vertex] = True

    if cycle_size < ceil(n / 2):
        raise RuntimeError("Should not reach here. Should never get small cycle.")
    elif cycle_size == n - 1:
        msg and setattr(msg, 'case', "Find cycle of size $n$ from one of size $n-1$ (hard!)")
        def find_adjacency(src: int, color: int, pos: List[int]) -> List[int]:
            ans = []
            for tgt in G.adjacency[color][src]:
                assert pos[tgt] != -1
                ans.append(pos[tgt])
            return ans

        def find_answer(vertices: List[Vertex], edges: List[Edge], i):
            vertices = vertices[i + 1:] + vertices[:i + 1] + [y]
            edges = edges[i + 1:] + edges[:i] + [G.get_edge(vertices[-2], y, pos_color_cic[i]), G.get_edge(y, vertices[0], cy)]
            return Cycle(G, vertices, edges)
        
        new_color_id = [-1] * n
        y = next(i for i in range(n) if not vertices_in_cycle[i])
        cy = next(i for i in range(n) if not colors_in_cycle[i])
        new_color_id[cy] = n - 1

        cycle_sz = cycle.size()
        for i in range(cycle_sz):
            color = cycle.edges[i].color
            new_color_id[color] = i

        vertex_position_on_cycle = [-1] * n
        pos_color_cic = [-1] * n
        for i in range(cycle_sz):
            vertex_position_on_cycle[cycle.vertices[i]] = i
            pos_color_cic[i] = cycle.edges[i].color

        I = []
        in_degree = [0] * n
        incoming_neighborhood = [[] for _ in range(n)]

        for i in range(cycle_sz):
            u, v = cycle.vertices[i], cycle.vertices[(i + 1) % cycle_sz]
            color = pos_color_cic[i]

            assert new_color_id[color] == i
            for tgt in G.adjacency[color][u]:
                if tgt == v:
                    continue

                in_degree[tgt] += 1
                incoming_neighborhood[tgt].append(u)

                if tgt == y:
                    I.append(i)

        _I = find_adjacency(y, cy, vertex_position_on_cycle)
        _I = [(u - 1 + cycle_sz) % cycle_sz for u in _I]

        for i in I:
            if i in _I:
                return find_answer(cycle.vertices, cycle.edges, i)
        
        vertices = cycle.vertices.copy()
        edges = cycle.edges.copy()

        found_vertex = False
        for i in range(cycle_sz):
            u = vertices[i]
            if in_degree[u] > n / 2 - 1:
                found_vertex = True
                vertices = vertices[i:] + vertices[:i]  # Rotaciona os vértices
                edges = edges[i:] + edges[:i]  # Rotaciona as arestas


                # Reconstrói o que precisa
                for j in range(cycle_sz):
                    color = edges[j].color
                    new_color_id[color] = j
                    vertex_position_on_cycle[vertices[j]] = j
                    pos_color_cic[j] = color
                break

        if found_vertex:
            I1 = find_adjacency(y, pos_color_cic[0], vertex_position_on_cycle)
            In = find_adjacency(y, cy, vertex_position_on_cycle)
            In = [(u - 1 + cycle_sz) % cycle_sz for u in In]

            assert len(I1) + len(In) >= n

            J = -1
            removed_color = -1
            new_vertices = []
            new_edges = []
            new_pos = [0] * n

            for i in I1:
                if i in In:
                    if i == 0:
                        return find_answer(vertices, edges, 0)

                    J = i
                    removed_color = pos_color_cic[J]
                    # Constrói o novo caminho
                    new_vertices = vertices[1:i+1] + [y] + vertices[i+1:cycle_sz] + [vertices[0]]

                    for j in range(len(new_vertices)):
                        new_pos[new_vertices[j]] = j

                    new_edges = edges[1:i] + [
                        G.get_edge(vertices[i], y, pos_color_cic[0]),
                        G.get_edge(y, vertices[(i + 1) % len(vertices)], cy)
                    ] + edges[i+1:cycle_sz]

                    assert len(new_vertices) == n and len(new_vertices) == len(new_edges) + 1
                    break
            # Caso em que fecha o ciclo bonitinho
            edge = G.check_edge(new_vertices[0], new_vertices[-1], removed_color)
            if edge is not None:
                new_edges.append(edge)
                return Cycle(cycle.G, new_vertices, new_edges)

            # Vamos achar J1 e Jn
            J1 = find_adjacency(new_vertices[0], removed_color, new_pos)
            J1 = [(u - 1 + n) % n for u in J1]

            Jn = incoming_neighborhood[vertices[0]]
            Jn = [new_pos[u] for u in Jn]

            for i in J1:
                if i in Jn:
                    # Achamos a resposta
                    if new_vertices[i + 1] == y:
                        continue
                    final_vertices = new_vertices[:i+1] + new_vertices[i+1:][::-1]
                    assert len(final_vertices) == n

                    final_edges = new_edges[:i] + [
                        G.get_edge(new_vertices[i], new_vertices[-1], new_edges[i].color)
                    ] + new_edges[i+1:][::-1] + [
                        G.get_edge(new_vertices[i+1], new_vertices[0], removed_color)
                    ]

                    return Cycle(cycle.G, final_vertices, final_edges)
        else:
            return handle_cycle_n_minus_1_upper_bound_degree_D(G, cycle, y, cy, in_degree, incoming_neighborhood)

        raise RuntimeError("Should not reach here. Did not find answer :(")
    else:
        for i in [miss1, miss2]:
            for u in range(n):
                if not vertices_in_cycle[u]:
                    # Itera sobre todas as arestas que saem de 'u' no grafo de cor i
                    for v in  G.adjacency[i][u]:
                        if vertices_in_cycle[v]:
                            continue
                        
                        # Aresta uv de cor j fora do ciclo, disjunta com o ciclo
                        j = miss2 if i == miss1 else miss1
                        
                        for w in G.adjacency[j][v]:
                            if not vertices_in_cycle[w]:
                                continue
                            
                            # "Anexa" o caminho uvw ao ciclo, gerando um caminho de tamanho l + 1
                            vertices = cycle.vertices.copy()
                            edges = cycle.edges.copy()
                            
                            w_id = vertices.index(w)
                            assert vertices[w_id] == w
                            
                            # Coloca w como o último vértice
                            vertices = vertices[w_id + 1:] + vertices[:w_id + 1] + [v, u]
                            edges = edges[w_id + 1:] + edges[:w_id] + [G.get_edge(w, v, j), G.get_edge(v, u, i)]

                            msg and setattr(msg, 'case', "There are two vertices outside the cycle that create a bigger path")
                            return Path(G, vertices, edges)  # Caminho de tamanho l + 1

        # Agora, para cada vértice não-u no ciclo, todas as arestas das cores miss1 e miss2
        # incidentes em 'u' devem ser incidentes ao ciclo
        for u in range(n):
            if not vertices_in_cycle[u]:
                vertices = cycle.vertices.copy()
                edges = cycle.edges.copy()
                
                for i in range(len(vertices)):
                    x1, x2 = vertices[i], vertices[(i + 1) % len(vertices)]
                    
                    for c1, c2 in [(miss1, miss2), (miss2, miss1)]:
                        edge1 = G.check_edge(u, x1, c1)
                        edge2 = G.check_edge(u, x2, c2)
                        
                        if (edge1 is not None) and (edge2 is not None):
                            vertices = vertices[i + 1:] + vertices[:i + 1] + [u]
                            edges = edges[i + 1:] + edges[:i] + [edge1, edge2]
                            
                            msg and setattr(msg, 'case', "We can remove a edge and add a vertex, creating a bigger cycle")
                            return Cycle(G, vertices, edges)  # Ciclo de tamanho l + 1

        raise RuntimeError("Código inacessível!")

def handle_cycle_n_minus_1_upper_bound_degree_D(G: Graph, cycle: Cycle, y: int, cy: int, in_degree: List[int], incoming_neighborhood: List[List[int]]):
    n = G.n
    Q = None
    removed_color = -1

    for j in range(2, n - 1):
        edge = G.check_edge(y, cycle.vertices[(j + 1) % len(cycle.vertices)], cy)
        if edge is None or in_degree[cycle.vertices[j]] != n // 2 - 1:
            continue
        Q_vertices = [y] + cycle.vertices[j + 1:] + cycle.vertices[:j + 1]
        Q_edges = [edge] + cycle.edges[j + 1:] + cycle.edges[:j]
        removed_color = cycle.edges[j].color
        Q = Path(G, Q_vertices, Q_edges)
        break

    if Q is None:
        raise RuntimeError("Did not find Path Q :(")

    edge = G.check_edge(Q.vertices[-1], Q.vertices[0], removed_color)
    if edge is not None:
        return Cycle(G, Q.vertices, Q.edges + [edge])

    J0 = []
    for i in range(n - 2):
        edge = G.check_edge(Q.vertices[0], Q.vertices[i + 1], removed_color)
        if edge is not None:
            J0.append((i, edge))

    position = [0] * n
    for i in range(n):
        position[Q.vertices[i]] = i

    inJn = [False] * n
    for v in incoming_neighborhood[Q.vertices[-1]]:
        inJn[position[v]] = True

    for k, edge_from_first in J0:
        if not inJn[k]:
            continue
        edge_from_last = G.get_edge(Q.vertices[n - 1], Q.vertices[k], Q.edges[k].color)
        
        new_vertices = Q.vertices[:k+1] + Q.vertices[k+1:][::-1]
        new_edges = Q.edges[:k] + [edge_from_last] + Q.edges[k+1:][::-1] + [edge_from_first]

        return Cycle(G, new_vertices, new_edges)

    raise RuntimeError("Not found")

def test_handle_cycle_n_minus_1_upper_bound_degree_D():
    # Create a graph with n=6 vertices
    n = 6

    # Create a cycle of size n-1 (5 vertices)
    cycle_vertices = [0, 1, 2, 3, 4]  # y will be vertex 5
    cycle_edges = [
        Edge(0, 1, 0),
        Edge(1, 2, 1),
        Edge(2, 3, 2),
        Edge(3, 4, 3),
        Edge(4, 0, 4)
    ]

    incoming_neighborhood = [
        [2, 3], # 0
        [3, 4], # 1
        [4, 0], # 2
        [1],    # 3
        [2],    # 4
        [0, 1], # 5
    ]
    in_degree = [2, 2, 2, 1, 1, 2]

    # we just need to build G with the edges of the digraph and cycle
    graph_edges = [
        # Edges of the cycle and digraph
        Edge(0, 1, 0),
        Edge(0, 2, 0),
        Edge(0, 5, 0),
        Edge(1, 2, 1),
        Edge(1, 3, 1),
        Edge(1, 5, 1),
        Edge(2, 3, 2),
        Edge(2, 4, 2),
        Edge(2, 0, 2),
        Edge(3, 4, 3),
        Edge(3, 0, 3),
        Edge(3, 1, 3),
        Edge(4, 0, 4),
        Edge(4, 1, 4),
        Edge(4, 2, 4),

        # Edges to complete Dirac condition
        # For graph color 0
        Edge(3, 4, 0),
        Edge(1, 2, 0),
        Edge(2, 3, 0),
        Edge(3, 4, 0),
        Edge(4, 5, 0),
        Edge(5, 1, 0),

        # For graph color 1
        Edge(4, 0, 1),
        Edge(0, 2, 1),
        Edge(2, 3, 1),
        Edge(3, 4, 1),
        Edge(4, 5, 1),
        Edge(5, 0, 1),

        # For graph color 2
        Edge(1, 5, 2),
        Edge(0, 1, 2),
        Edge(1, 3, 2),
        Edge(3, 4, 2),
        Edge(4, 5, 2),
        Edge(5, 0, 2),

        # For graph color 3
        Edge(2, 5, 3),
        Edge(0, 1, 3),
        Edge(1, 2, 3),
        Edge(2, 4, 3),
        Edge(4, 5, 3),
        Edge(5, 0, 3),
        
        # For graph color 4
        Edge(3, 5, 4),
        Edge(0, 1, 4),
        Edge(1, 2, 4),
        Edge(2, 3, 4),
        Edge(3, 5, 4),
        Edge(5, 0, 4),

        # For graph color 5
        Edge(0, 1, 5),
        Edge(1, 2, 5),
        Edge(2, 3, 5),
        Edge(3, 4, 5),
        Edge(4, 5, 5),
        Edge(5, 0, 5),

        Edge(3, 5, 5),
        Edge(0, 2, 5),
        Edge(1, 4, 5),
    ]

    G = Graph(n)
    for edge in graph_edges:
        G.add_edge(edge.u, edge.v, edge.color)

    cycle = Cycle(G, cycle_vertices, cycle_edges)

    y = 5
    cy = 5

    # Lets verify the digraph we built is correct
    for i in range(n):
        for color in range(n):
            assert len(G.adjacency[color][i]) >= n // 2
        assert len(incoming_neighborhood[i]) == in_degree[i]
        assert in_degree[i] <= n // 2 - 1

    result = handle_cycle_n_minus_1_upper_bound_degree_D(G, cycle, y, cy, in_degree, incoming_neighborhood)
    
    assert isinstance(result, Cycle)
    assert len(result.vertices) == n  # Should have all n vertices
    assert len(result.edges) == n     # Should have n edges

