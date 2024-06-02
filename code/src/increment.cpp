#include "increment.hpp"

// Given a Path
std::variant<graph::Cycle, graph::Path> increment_path(const graph::Instance& instance, graph::Path path) {
    const auto& GG = *instance.first;
    const auto G = instance.second;

    auto n = boost::num_vertices(GG);
    
    if (path.size() == 0) {
        throw std::runtime_error("Path is empty.");
    }

    int lastVertex = path.back();

    std::vector<int> usedColors(n, 0), usedVertices(n, 0);
    for (const auto&edge : path.edges) {
        usedColors[GG[edge].color]++;
    }
    for (const auto&vertex : path.vertices) {
        usedVertices[vertex]++;
    }
    if (path.size() < (n + 1) / 2) {
        // esse caso eh so adicionar uma cor que nao tem ainda
        graph::Path newPath(instance.first);
        newPath.vertices = path.vertices;
        newPath.edges = path.edges;
        for (int color = 0; color < n; color++) if (usedColors[color] == 0) {
            for (int i = 0; i < n; i++) if (usedVertices[i] == 0) {
                if (newPath.push_back(i)) {
                    return newPath;
                }
            }
        }
        assert(false);
    }
    else {
        // esse eh o caso de adicionar uma cor com crossing

        // iterate through adjacenct vertices of y
        // for (const auto& edge : boost::make_iterator_range(boost::out_edges(y, GG))) {
        //     int u = boost::target(edge, GG);
        //     if (GG[edge].color == cy && !usedVertices[u]) {
        //         path.push_back(u);
        //         return path;
        //     }
        // }

        int cx = GG[path.edges.back()].color, cy = -1;

        for (int i = 0; i < n; i++) if (!usedColors[i]) {
            cy = i;
            break;
        }

        path.pop_back();
        int x = path.vertices[0], y = path.vertices.back();

        
        // vamos ver se sao adjacentes da cor cx ou cy
        for (auto c : {cx, cy}) {
            auto [b, edge] = graph::checkEdge(x, y, c, GG);
            if (b) {
                graph::Cycle cycle(instance.first, edge);
                return cycle;
            }
        }
        // verificamos se nao tem um vertice que seja adjacente a x e y nas cores cx e cy
        for (int i = 0; i < n; i++) if (!usedVertices[i]) {
            auto [bX, edgeX] = graph::checkEdge(x, i, cx, GG);
            auto [bY, edgeY] = graph::checkEdge(y, i, cy, GG);
            if (bX && bY) {
                path.push_back(i);
                graph::Cycle cycle(instance.first, edgeX);
                return cycle;
            }
        }
        // quantidade de vertices
        int l = path.size() + 1;
        std::vector<int> I1 (n), I2(n);
        
        for (int i = 1; i < path.size() - 1; i++) {
            int u = path.vertices[i], v = path.vertices[i + 1];
            auto [bX, edgeX] = checkEdge(x, v, cx);
            auto [bY, edgeY] = checkEdge(u, y, cy);

            if (bX && bY) {
                // achamos um crossing
                // graph::Path newPath;
            }
        }
    }
    

    
    return graph::Cycle(instance.first);
}
std::variant<graph::Cycle, graph::Path> increment_cycle(const graph::Instance& instance, const graph::Cycle& cycle);
