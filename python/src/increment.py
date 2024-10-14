from graph import *
from math import ceil
from pprint import pprint

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

    if path.size() < ceil(n / 2) + 1:
        vertices = path.vertices
        edges = path.edges
        final_vertex = path.back()
        for color in range(n):
            if not colors_in_path[color]:
                for i in range(n):
                    if not vertices_in_path[i]:
                        b, edge = G.check_edge(final_vertex, i, color)
                        if b:
                            vertices.append(i)
                            edges.append(edge)
                            return Path(G, vertices, edges)
        raise RuntimeError("Should not reach here. Small path should return a new path.")
    else:
        cx = path.edges[-1].color
        cy = next(i for i in range(n) if not colors_in_path[i])


        path.pop_back()
        x, y = path.vertices[0], path.vertices[-1]
        print("cx, cy, x, y", cx, cy, x, y)

        for c in [cx, cy]:
            b, edge = G.check_edge(x, y, c)
            if b:
                vertices = path.vertices.copy()
                edges = path.edges.copy()
                edges.append(edge)
                return Cycle(G, vertices, edges)

        for i in range(n):
            if not vertices_in_path[i]:
                bX, edgeX = G.check_edge(x, i, cx)
                bY, edgeY = G.check_edge(y, i, cy)
                if bX and bY:
                    vertices = path.vertices.copy()
                    vertices.append(i)
                    edges = path.edges.copy()
                    edges.append(edgeY)
                    edges.append(edgeX)
                    return Cycle(G, vertices, edges)

        for i in range(1, len(path.vertices) - 1):
            u, v = path.vertices[i], path.vertices[i + 1]
            bX, edgeX = G.check_edge(x, v, cx)
            bY, edgeY = G.check_edge(u, y, cy)

            if bX and bY:
                vertices = path.vertices[:i + 1] + [y] + path.vertices[i + 1:]
                edges = path.edges[:i] + [edgeY] + path.edges[i:]
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

    if cycle_size < ceil(n / 2) + 1:
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

        pos_cic = [-1] * n
        pos_color_cic = [-1] * n
        for i in range(cycle_sz):
            pos_cic[cycle.vertices[i]] = i
            pos_color_cic[i] = cycle.edges[i].color

        I = []
        in_degree = [0] * n
        incoming_neighborhood = [[] for _ in range(n)]

        for i in range(cycle_sz):
            u, v = cycle.vertices[i], cycle.vertices[(i + 1) % cycle_sz]
            color = pos_color_cic[i]

            for tgt in G.adjacency[color][u]:
                if tgt == v:
                    continue
                assert new_color_id[color] == i

                in_degree[tgt] += 1
                incoming_neighborhood[tgt].append(u)

                if tgt == y:
                    I.append(i)

        def find_adjacency(src, color, pos):
            ans = []
            for tgt in G.adjacency[color][src]:
                assert pos[tgt] != -1
                ans.append(pos[tgt])
            return ans

        _I = find_adjacency(y, cy, pos_cic)
        _I = [(u - 1 + cycle_sz) % cycle_sz for u in _I]

        def find_answer(vertices: List[Vertex], edges: List[Edge], i):
            vertices = vertices[i + 1:] + vertices[:i + 1]
            edges = edges[i + 1:] + edges[:i + 1]
            edges.pop()

            _, edge = G.check_edge(vertices[-1], y, pos_color_cic[i])
            _, edge2 = G.check_edge(y, vertices[0], cy)

            vertices.append(y)
            edges.append(edge)
            edges.append(edge2)

            return Cycle(G, vertices, edges)

        for i in I:
            if i in _I:
                return find_answer(cycle.vertices.copy(), cycle.edges.copy(), i)

        raise RuntimeError("Should not reach here. Did not find answer.")
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
                            edges = edges[w_id + 1:] + edges[:w_id + 1]
                            edges.pop()  # Remove a última aresta

                            # Cria um novo caminho com o vértice e arestas
                            vertices.append(v)
                            edges.append(G.check_edge(w, v, j)[1])
                            vertices.append(u)
                            edges.append(G.check_edge(v, u, i)[1])
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
                        exists1, edge1 = G.check_edge(u, x1, c1)
                        exists2, edge2 = G.check_edge(u, x2, c2)
                        
                        if exists1 and exists2:
                            vertices = vertices[i + 1:] + vertices[:i + 1]
                            edges = edges[i + 1:] + edges[:i + 1]
                            edges.pop()  # Remove a última aresta
                            edges.append(edge1)
                            vertices.append(u)
                            edges.append(edge2)
                            
                            return Cycle(G, vertices, edges)  # Ciclo de tamanho l + 1

        raise RuntimeError("Código inacessível!")

