from graph import *
from math import ceil
from typing import Union

def increment(G: Graph, path_or_cycle: Union[Path, Cycle]) -> Union[Path, Cycle]:
    if isinstance(path_or_cycle, Path):
        return handle_path(G, path_or_cycle)
    elif isinstance(path_or_cycle, Cycle):
        return handle_cycle(G, path_or_cycle)

def handle_path(G: Graph, path: Path):
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
        vertices = path.vertices
        edges = path.edges
        final_vertex = path.back()

        color_outside_path = next(i for i in range(n) if not colors_in_path[i])

        for i in range(n):
            if not vertices_in_path[i]:
                edge = G.check_edge(final_vertex, i, color_outside_path)
                if edge is not None:
                    vertices.append(i)
                    edges.append(edge)
                    return Path(G, vertices, edges)
        raise RuntimeError("Should not reach here. Small path should return a new path.")
    else:
        cx = path.edges[-1].color
        cy = next(i for i in range(n) if not colors_in_path[i])
        path.pop_back()
        x, y = path.vertices[0], path.vertices[-1]

        for c in [cx, cy]:
            edge = G.check_edge(x, y, c)
            if edge is not None:
                vertices = path.vertices.copy()
                edges = path.edges.copy() + [edge]
                return Cycle(G, vertices, edges)

        for [c1, c2] in [[cx, cy], [cy, cx]]:
            for i in range(n):
                if not vertices_in_path[i]:
                    edgeX = G.check_edge(x, i, c1)
                    edgeY = G.check_edge(y, i, c2)
                    if (edgeX is not None) and (edgeY is not None):
                        vertices = path.vertices.copy() + [i]
                        edges = path.edges.copy() + [edgeY, edgeX]
                        return Cycle(G, vertices, edges)

            for i in range(1, len(path.vertices) - 1):
                u, v = path.vertices[i], path.vertices[i + 1]
                edgeX = G.check_edge(x, v, c1)
                edgeY = G.check_edge(u, y, c2)

                if (edgeX is not None) and (edgeY is not None):
                    vertices = path.vertices[:i + 1] + [y]
                    edges = path.edges[:i] + [edgeY]
                    for j in range(len(path.vertices) - 1, i + 1, -1):
                        vertices.append(path.vertices[j - 1])
                        edges.append(path.edges[j - 1])
                    edges.append(edgeX)
                    return Cycle(G, vertices, edges)

    raise RuntimeError("Should not reach here. Did not find crossing.")

def handle_cycle(G: Graph, cycle: Cycle):
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
        
        def find_adjacency(src: int, color: int, pos: List[int]) -> List[int]:
            ans = []
            for tgt in G.adjacency[color][src]:
                assert pos[tgt] != -1
                ans.append(pos[tgt])
            return ans

        _I = find_adjacency(y, cy, vertex_position_on_cycle)
        _I = [(u - 1 + cycle_sz) % cycle_sz for u in _I]

        def find_answer(vertices: List[Vertex], edges: List[Edge], i):
            vertices = vertices[i + 1:] + vertices[:i + 1]
            edges = edges[i + 1:] + edges[:i + 1]
            edges.pop()

            edge = G.get_edge(vertices[-1], y, pos_color_cic[i])
            edge2 = G.get_edge(y, vertices[0], cy)
            vertices.append(y)
            edges.append(edge)
            edges.append(edge2)

            return Cycle(G, vertices, edges)

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
            def get_Q_and_cj():
                for j in range(n - 1):
                    edge = G.get_edge(y, cycle.vertices[j], cy)
                    if edge is None or in_degree[cycle.vertices[j]] != n // 2 - 1:
                        continue
                    new_vertices = cycle.vertices[:]
                    new_edges = cycle.edges[:]
                    new_vertices = new_vertices[j + 1:] + new_vertices[:j + 1]
                    new_edges = new_edges[j + 1:] + new_edges[:j + 1]
                    cj = new_edges[-1].color
                    new_edges.pop()
                    new_vertices.insert(0, y)
                    new_edges.insert(0, edge)
                    return Path(G, new_vertices, new_edges), cj
                raise RuntimeError("j not found!")

            Q, cj = get_Q_and_cj()

            # assert that Q misses cj
            for edge in Q.edges:
                assert edge.color != cj

            x = Q.vertices

            edge = G.get_edge(x[n - 2], x[0], cj)
            if edge is not None:
                edges = Q.edges[:]
                edges.append(edge)
                return Cycle(G, vertices, edges)

            J1 = []
            for i in range(n - 2):
                edge = G.get_edge(x[0], x[i + 1], cj)
                J1.append((i, edge))

            position = [0] * n
            for i in range(n):
                position[x[i]] = i

            inJn = [False] * n
            for v in incoming_neighborhood[x[n - 1]]:
                inJn[position[v]] = True

            for k, edge_from_first in J1:
                color = Q.edges[k].color
                edge_from_last = G.check_edge(x[n - 1], x[k], color)
                if edge_from_last is None:
                    continue

                new_vertices = [x[0], x[k + 1]]
                new_edges = [edge_from_first]

                for i in range(k + 1, n - 1):
                    new_vertices.append(x[i + 1])
                    new_edges.append(Q.edges[i])

                new_edges.append(edge_from_last)

                for i in range(k, 0, -1):
                    edge = G.get_edge(x[i], x[i - 1], Q.edges[i - 1].color)
                    new_edges.append(edge)

                return Cycle(G, new_vertices, new_edges)

            raise RuntimeError("Not found")


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
                            vertices = vertices[w_id + 1:] + vertices[:w_id + 1]
                            edges = edges[w_id + 1:] + edges[:w_id]

                            # Cria um novo caminho com o vértice e arestas
                            vertices.append(v)
                            edges.append(G.get_edge(w, v, j))
                            vertices.append(u)
                            edges.append(G.get_edge(v, u, i))
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
                            vertices = vertices[i + 1:] + vertices[:i + 1]
                            edges = edges[i + 1:] + edges[:i + 1]
                            edges.pop()  # Remove a última aresta
                            edges.append(edge1)
                            vertices.append(u)
                            edges.append(edge2)
                            
                            return Cycle(G, vertices, edges)  # Ciclo de tamanho l + 1

        raise RuntimeError("Código inacessível!")

