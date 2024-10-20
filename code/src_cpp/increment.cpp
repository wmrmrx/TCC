#include "increment.hpp"
#include "graph.hpp"
#include "util.hpp"

using Variant = std::variant<graph::Cycle, graph::Path>;

struct Visitor
{
    const graph::Instance &instance;
    Variant operator()(graph::Path path)
    {
        const auto &GG = *instance.first;

        auto n = boost::num_vertices(GG);

        if (path.vertices.size() == 0)
        {
            throw std::runtime_error("Path is empty.");
        }

        std::vector<bool> colorsInPath(n, false), verticesInPath(n, false);
        for (auto &edge : path.edges)
        {
            colorsInPath[graph::color(GG, edge)] = true;
        }
        for (auto &vertex : path.vertices)
        {
            verticesInPath[vertex] = true;
        }
        if (path.size() < ceil_div((uint64_t)n, uint64_t(2)) + 1)
        {
            // esse caso eh so adicionar uma cor que nao tem ainda
            auto &vertices = path.vertices;
            auto &edges = path.edges;
            int finalVertex = path.back();
            for (size_t color = 0; color < n; color++)
                if (colorsInPath[color] == 0)
                {
                    for (size_t i = 0; i < n; i++)
                        if (verticesInPath[i] == 0)
                        {
                            auto [b, edge] = graph::checkEdge(finalVertex, i, color, instance);
                            if (b)
                            {
                                vertices.push_back(i);
                                edges.push_back(edge);
                                return graph::Path(instance.first, vertices, edges);
                            }
                        }
                }
            throw std::runtime_error("Should not reach here. Small path should return a new path.");
        }
        else
        {
            // temos duas cores que nao foram usadas
            int cx = GG[path.edges.back()].color, cy = -1;

            for (size_t i = 0; i < n; i++)
                if (!colorsInPath[i])
                {
                    cy = i;
                    break;
                }

            // tiramos a ultima aresta
            path.pop_back();
            int x = path.vertices[0], y = path.vertices.back();

            // vamos ver se sao adjacentes da cor cx ou cy
            // se forem, entao temos um ciclo com o mesmo tamanho do path original
            // embaixo de (1) no pdf
            for (auto c : {cx, cy})
            {
                auto [b, edge] = graph::checkEdge(x, y, c, instance);
                if (b)
                {
                    auto vertices = path.vertices;
                    auto edges = path.edges;
                    edges.push_back(edge);
                    return graph::Cycle(instance.first, vertices, edges);
                }
            }
            // verificamos se nao tem um vertice que seja adjacente a x e y nas cores cx e cy
            for (int k = 0; k < 2; k++) {
                for (size_t i = 0; i < n; i++)
                    if (!verticesInPath[i])
                    {
                        auto [bX, edgeX] = graph::checkEdge(x, i, k ? cx : cy, instance);
                        auto [bY, edgeY] = graph::checkEdge(y, i, k ? cy : cx, instance);
                        if (bX && bY)
                        {
                            auto vertices = path.vertices;
                            vertices.push_back(i);
                            auto edges = path.edges;

                            edges.push_back(edgeY);
                            edges.push_back(edgeX);
                            return graph::Cycle(instance.first, vertices, edges);
                        }
                    }
                for (size_t i = 1; i < path.vertices.size() - 1; i++)
                {
                    int u = path.vertices[i], v = path.vertices[i + 1];
                    auto [bX, edgeX] = graph::checkEdge(x, v, k ? cx : cy, instance);
                    auto [bY, edgeY] = graph::checkEdge(u, y, k ? cy : cx, instance);

                    if (bX && bY)
                    {
                        // achamos um crossing
                        std::vector<graph::Vertex> vertices;
                        std::vector<graph::Edge> edges;

                        for (size_t j = 0; j <= i; j++)
                        {
                            vertices.push_back(path.vertices[j]);
                            if (j)
                                edges.push_back(path.edges[j - 1]);
                        }
                        vertices.push_back(y);
                        edges.push_back(edgeY);
                        for (size_t j = path.vertices.size() - 1; j > i + 1; j--)
                        {
                            vertices.push_back(path.vertices[j - 1]);
                            edges.push_back(path.edges[j - 1]);
                        }
                        edges.push_back(edgeX);
                        return graph::Cycle(instance.first, vertices, edges);
                    }
                }
            }
        }
        throw std::runtime_error("Should not reach here. Did not found crossing.");
    }

    // Given a Path
    Variant operator()(const graph::Cycle &cycle)
    {
        const auto &GG = *instance.first;
        const auto &G = instance.second;

        const auto n = boost::num_vertices(GG);
        const uint64_t cycle_size = cycle.size();
        if (cycle_size < 3)
        {
            throw std::runtime_error("Every cycle must have at least 3 vertices");
        }
        std::vector<bool> colors_in_cycle(n), vertices_in_cycle(n);
        for (auto &e : cycle.edges)
            colors_in_cycle[GG[e].color] = true;
        size_t miss1 = 0, miss2 = n - 1;
        while (colors_in_cycle[miss1])
            miss1++;
        while (colors_in_cycle[miss2])
            miss2--;
        assert(miss1 < miss2 || cycle_size == n - 1);

        for (auto &v : cycle.vertices)
            vertices_in_cycle[v] = true;

        if (cycle_size < ceil_div((uint64_t)n, uint64_t(2)) + 1)
        {
            throw std::runtime_error("Should not reach here. Should never get small cycle.");
        }
        else if (cycle_size == n - 1)
        {
            std::vector<int> newColorId(n);
            // y eh o vertice que nao esta no ciclo
            // fazemos o relabel das cores
            size_t y, cy;
            size_t cycle_sz = cycle.size();
            for (size_t i = 0; i < n; i++)
            {
                if (!vertices_in_cycle[i])
                    y = i;
                if (!colors_in_cycle[i])
                    cy = i, newColorId[i] = n - 1;
            }
            for (size_t i = 0; i < cycle_sz; i++)
            {
                auto color = GG[cycle.edges[i]].color;
                newColorId[color] = i;
            }

            std::vector<int> posCic(n, -1), posColorCic(n, -1);
            for (size_t i = 0; i < cycle_sz; i++)
            {
                posCic[cycle.vertices[i]] = i;
                posColorCic[i] = GG[cycle.edges[i]].color;
            }

            // vamos montar o digrafo desse cara
            std::vector<size_t> I, _I;
            std::vector<size_t> inDegree(n), outDegree(n);
            std::vector<std::vector<graph::Vertex>> incomingNeighborhood(n);

            // aqui vamos achar os indices dos vertice no ciclo que ligam com y
            for (size_t i = 0; i < cycle_sz; i++)
            {
                size_t u = cycle.vertices[i], v = cycle.vertices[(i + 1) % cycle_sz], color = posColorCic[i];

                for (const auto &edge : boost::make_iterator_range(boost::out_edges(u, G[color])))
                {
                    auto src = boost::source(edge, G[color]), tgt = boost::target(edge, G[color]);
                    // tiramos arestas de u pra v
                    if (tgt == v)
                        continue;
                    assert(u == src && (size_t)newColorId[color] == i);

                    outDegree[u]++;
                    inDegree[tgt]++;
                    incomingNeighborhood[tgt].push_back(u);

                    // isso eh la pro final
                    if (tgt == y)
                        I.push_back(i);
                }
            }

            auto findAdjacency = [&](int src, int color, std::vector<int> &pos) -> std::vector<size_t>
            {
                std::vector<size_t> ans;
                for (const auto &edge : boost::make_iterator_range(boost::out_edges(src, G[color])))
                {
                    auto tgt = boost::target(edge, G[color]);
                    assert((size_t)src == boost::source(edge, G[color]) && pos[tgt] != -1);
                    ans.push_back(pos[tgt]);
                }
                return ans;
            };
            _I = findAdjacency(y, cy, posCic);
            for (auto &u : _I)
                u = (u - 1 + cycle_sz) % cycle_sz;

            // aqui eh se achamos uma resposta que vai tirar a aresta i do ciclo
            auto find_answer = [&](std::vector<graph::Vertex> vertices, std::vector<graph::Edge> edges, int i)
            {
                std::rotate(vertices.begin(), vertices.begin() + (i + 1), vertices.end());
                std::rotate(edges.begin(), edges.begin() + (i + 1), edges.end());

                edges.pop_back();

                auto [_, edge] = graph::checkEdge(vertices.back(), y, posColorCic[i], instance);
                auto [__, edge2] = graph::checkEdge(y, vertices[0], cy, instance);

                assert(_ && __);

                vertices.push_back(y);
                edges.push_back(edge);
                edges.push_back(edge2);

                return graph::Cycle(cycle.G, vertices, edges);
            };

            for (auto i : I)
            {
                // achamos a resposta final. vai ser basicamente o ciclo, mas com a aresta de x_i pra y com cor i
                // e de y pra x_{i+1} com cor n - 1
                if (std::find(_I.begin(), _I.end(), i) != _I.end())
                {
                    return find_answer(cycle.vertices, cycle.edges, i);
                }
            }
            auto vertices = cycle.vertices;
            auto edges = cycle.edges;

            bool foundVertex = false;
            for (size_t i = 0; i < cycle_sz; i++)
            {
                int u = vertices[i];
                if (inDegree[u] > n / 2 - 1)
                {
                    foundVertex = true;
                    std::rotate(vertices.begin(), vertices.begin() + i, vertices.end());
                    std::rotate(edges.begin(), edges.begin() + i, edges.end());

                    // reconstruimos o que precisa
                    for (size_t i = 0; i < cycle_sz; i++)
                    {
                        auto color = GG[edges[i]].color;
                        newColorId[color] = i;

                        posCic[vertices[i]] = i;
                        posColorCic[i] = GG[edges[i]].color;
                    }
                    break;
                }
            }
            if (foundVertex)
            {
                auto I1 = findAdjacency(y, posColorCic[0], posCic);
                auto In = findAdjacency(y, cy, posCic);
                for (auto &u : In)
                    u = (u - 1 + cycle_sz) % cycle_sz;

                assert(I1.size() + In.size() >= n);

                int J = -1, removedColor = -1;
                std::vector<graph::Vertex> newVertices;
                std::vector<graph::Edge> newEdges;
                std::vector<int> newPos(n);
                for (auto i : I1)
                {
                    if (std::find(In.begin(), In.end(), i) != In.end())
                    {
                        if (i == 0)
                            return find_answer(vertices, edges, 0);
                        J = i;
                        removedColor = posColorCic[J];
                        // construimos o novo path desse desgracado
                        for (size_t j = 1; j <= i; j++)
                            newVertices.push_back(vertices[j]);
                        newVertices.push_back(y);
                        for (size_t j = i + 1; j < cycle_sz; j++)
                            newVertices.push_back(vertices[j]);
                        newVertices.push_back(vertices[0]);

                        for (size_t j = 0; j < newVertices.size(); j++)
                            newPos[newVertices[j]] = j;

                        for (size_t j = 1; j < i; j++)
                            newEdges.push_back(edges[j]);
                        {
                            auto [_, edge] = graph::checkEdge(vertices[i], y, posColorCic[0], instance);
                            auto [__, edge2] = graph::checkEdge(y, vertices[(i + 1) % vertices.size()], cy, instance);
                            assert(_ && __);
                            newEdges.push_back(edge);
                            newEdges.push_back(edge2);
                        }
                        for (size_t j = i + 1; j < cycle_sz; j++)
                            newEdges.push_back(edges[j]);
                        assert(newVertices.size() == n && newVertices.size() == newEdges.size() + 1);
                        break;
                    }
                }
                {
                    // esse eh o caso em que so fecha o ciclo bonitinho
                    auto [found, edge] = graph::checkEdge(newVertices[0], newVertices.back(), posColorCic[J], instance);
                    if (found)
                    {
                        newEdges.push_back(edge);
                        return graph::Cycle(cycle.G, newVertices, newEdges);
                    }
                }
                // vamos achar J1 e Jn
                auto J1 = findAdjacency(newVertices[0], posColorCic[J], newPos);
                for (auto &u : J1)
                    u = (u - 1 + n) % n;

                auto Jn = incomingNeighborhood[vertices[0]];
                for (auto &u : Jn)
                    u = newPos[u];

                for (auto i : J1)
                {
                    if (std::find(Jn.begin(), Jn.end(), i) != Jn.end())
                    {
                        // achamos uma resposta!!!
                        // pegamos o cara certo
                        if (newVertices[i + 1] == y)
                            continue;

                        std::vector<graph::Vertex> finalVertices;
                        std::vector<graph::Edge> finalEdges;
                        for (size_t j = 0; j <= i; j++)
                            finalVertices.push_back(newVertices[j]);
                        for (size_t j = n - 1; j > i; j--)
                            finalVertices.push_back(newVertices[j]);
                        assert(finalVertices.size() == n);

                        for (size_t j = 0; j < i; j++)
                            finalEdges.push_back(newEdges[j]);
                        {
                            auto [_, edge] = graph::checkEdge(newVertices[i], newVertices.back(), graph::color(GG, newEdges[i]), instance);
                            assert(_);
                            finalEdges.push_back(edge);
                        }
                        for (size_t j = n - 2; j > i; j--)
                            finalEdges.push_back(newEdges[j]);
                        {
                            auto [_, edge] = graph::checkEdge(newVertices[i + 1], newVertices[0], removedColor, instance);
                            assert(_);
                            finalEdges.push_back(edge);
                        }
                        return graph::Cycle(cycle.G, finalVertices, finalEdges);
                    }
                }
                throw std::runtime_error("Should not reach here. Did not find answer :(");
            }
            else
            {
                auto get_Q_and_cj = [&]()
                {
                    for (size_t j = 0; j < n - 1; j++)
                    {
                        auto [has_edge, edge] = graph::checkEdge(y, cycle.vertices[j], cy, instance);
                        if (not has_edge or inDegree[cycle.vertices[j]] != n / 2 - 1)
                            continue;
                        auto new_vertices = cycle.vertices;
                        auto new_edges = cycle.edges;
                        std::rotate(new_vertices.begin(), new_vertices.begin() + (j + 1), new_vertices.end());
                        std::rotate(new_edges.begin(), new_edges.begin() + (j + 1), new_edges.end());
                        size_t cj = GG[new_edges.back()].color;
                        new_edges.pop_back();
                        new_vertices.insert(new_vertices.begin(), y);
                        new_edges.insert(new_edges.begin(), edge);
                        return std::pair<graph::Path, size_t>(graph::Path(instance.first, new_vertices, new_edges), cj);
                    }
                    throw std::runtime_error("j not found!");
                };
                auto [Q, cj] = get_Q_and_cj();
                // assert that Q misses cj
                for (auto edge : Q.edges)
                    assert(GG[edge].color != cj);
                auto x = Q.vertices;
                {
                    auto [has_edge, edge] = checkEdge(x[n - 2], x[0], cj, instance);
                    if (has_edge)
                    {
                        auto edges = Q.edges;
                        edges.push_back(edge);
                        return graph::Cycle(instance.first, vertices, edges);
                    }
                }
                std::vector<std::pair<graph::Vertex, graph::Edge>> J1(n);
                for (size_t i = 0; i < n - 2; i++)
                {
                    auto [has_edge, edge] = checkEdge(x[0], x[i + 1], cj, instance);
                    J1.emplace_back(i, edge);
                }
                std::vector<size_t> position(n);
                for (size_t i = 0; i < n; i++)
                    position[x[i]] = i;
                std::vector<bool> inJn(n);
                for (auto v : incomingNeighborhood[x[n - 1]])
                {
                    inJn[position[v]] = true;
                }
                for (auto [k, edge_from_first] : J1)
                {
                    size_t color = GG[Q.edges[k]].color;
                    auto [has_edge, edge_from_last] = graph::checkEdge(x[n - 1], x[k], color, instance);
                    if (not has_edge)
                        continue;
                    std::vector<graph::Vertex> new_vertices;
                    std::vector<graph::Edge> new_edges;
                    new_vertices.push_back(x[0]);
                    new_vertices.push_back(x[k + 1]);
                    new_edges.push_back(edge_from_first);
                    for (size_t i = k + 1; i + 1 < n; i++)
                    {
                        new_vertices.push_back(x[i + 1]);
                        new_edges.push_back(Q.edges[i]);
                    }
                    new_edges.push_back(edge_from_last);
                    for (size_t i = k; i > 0; i--)
                    {
                        auto [_has_edge, edge] = graph::checkEdge(x[i], x[i - 1], GG[Q.edges[i - 1]].color, instance);
                        assert(_has_edge);
                        new_edges.push_back(edge);
                    }
                    return graph::Cycle(instance.first, new_vertices, new_edges);
                }
                throw std::runtime_error("Not found");
            }
        }
        else
        {
            for (size_t i : {miss1, miss2})
                for (size_t u = 0; u < n; u++)
                    if (not vertices_in_cycle[u])
                        for (const auto &u_edge : boost::make_iterator_range(boost::out_edges(u, G[i])))
                        {
                            assert(boost::source(u_edge, G[i]) == u);
                            auto v = boost::target(u_edge, G[i]);
                            if (vertices_in_cycle[v])
                                continue;
                            // uv is an edge of color j outside disjoint with the cycle
                            size_t j = i == miss1 ? miss2 : miss1;
                            for (const auto &v_edge : boost::make_iterator_range(boost::out_edges(v, G[j])))
                            {
                                auto w = boost::target(v_edge, G[j]);
                                if (not vertices_in_cycle[w])
                                    continue;
                                // "append" the path uvw to the cycle, generating a path of size l + 1
                                auto vertices = cycle.vertices;
                                auto edges = cycle.edges;
                                size_t w_id = std::find(vertices.begin(), vertices.end(), w) - vertices.begin();
                                assert(vertices[w_id] == w);
                                // make w the last vertex
                                std::rotate(vertices.begin(), vertices.begin() + (w_id + 1), vertices.end());
                                std::rotate(edges.begin(), edges.begin() + (w_id + 1), edges.end());
                                edges.pop_back();
                                graph::Path path(cycle.G);
                                vertices.push_back(v);
                                edges.push_back(graph::checkEdge(w, v, j, instance).second);
                                vertices.push_back(u);
                                edges.push_back(graph::checkEdge(v, u, i, instance).second);
                                return graph::Path(cycle.G, vertices, edges); // path of length l + 1
                            }
                        }
            // now, for every vertex not u in the cycle, all edges of colors miss1 and miss2 incident to u
            // must be incident to the cycle
            for (size_t u = 0; u < n; u++)
                if (not vertices_in_cycle[u])
                {
                    auto vertices = cycle.vertices;
                    auto edges = cycle.edges;
                    for (size_t i = 0; i < vertices.size(); i++)
                    {
                        auto x1 = vertices[i], x2 = vertices[(i + 1) % n];
                        for (auto [c1, c2] : {std::pair(miss1, miss2), std::pair(miss2, miss1)})
                        {
                            auto [exists1, edge1] = graph::checkEdge(u, x1, c1, instance);
                            auto [exists2, edge2] = graph::checkEdge(u, x2, c2, instance);
                            if (exists1 and exists2)
                            {
                                std::rotate(vertices.begin(), vertices.begin() + (i + 1), vertices.end());
                                std::rotate(edges.begin(), edges.begin() + (i + 1), edges.end());
                                edges.pop_back();
                                edges.push_back(edge1);
                                vertices.push_back(u);
                                edges.push_back(edge2);
                                return graph::Cycle(cycle.G, vertices, edges); // cycle of length l + 1
                            }
                        }
                    }
                }
            throw std::runtime_error("Unreachable code!");
        }
        throw std::runtime_error("Unreachable code!!!");
    }
};

Variant increment(const graph::Instance &instance, const Variant &state)
{
    return std::visit(Visitor{instance}, state);
}
