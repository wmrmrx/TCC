#include "increment.hpp"

// Given a Path
std::variant<graph::Cycle, graph::Path> increment_path(const graph::Instance& instance, const graph::Path& path) {
    const auto& GG = *instance.first;
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
        
    }
    

    
    return graph::Cycle(instance.first);
}
std::variant<graph::Cycle, graph::Path> increment_cycle(const graph::Instance& instance, const graph::Cycle& cycle);
