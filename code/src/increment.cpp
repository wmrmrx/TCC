#include "increment.hpp"
#include "graph.hpp"
#include "util.hpp"

using Variant = std::variant<graph::Cycle, graph::Path>;

struct Visitor {
	const graph::Instance& instance;
	Variant operator()(graph::Path path) {
	    const auto& GG = *instance.first;
	    const auto& G = instance.second;

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
		int finalVertex = path.back();
		for (int color = 0; color < n; color++) if (usedColors[color] == 0) {
		    for (int i = 0; i < n; i++) if (usedVertices[i] == 0) {
			auto [b, edge] = graph::checkEdge(finalVertex, i, color, GG);
			if (b) {
			    newPath.push_back(i, edge);
			    return newPath;
			}
		    }
		}
	    }
	    else {
		// temos duas cores que nao foram usadas
		int cx = GG[path.edges.back()].color, cy = -1;

		for (int i = 0; i < n; i++) if (!usedColors[i]) {
		    cy = i;
		    break;
		}

		// tiramos a ultima aresta
		path.pop_back();
		int x = path.vertices[0], y = path.vertices.back();

		
		// vamos ver se sao adjacentes da cor cx ou cy
		// se forem, entao temos um ciclo com o mesmo tamanho do path original
		// embaixo de (1) no pdf
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
			path.push_back(i, edgeY);
			graph::Cycle cycle(path, edgeX);
			return cycle;
		    }
		}

		for (int i = 1; i < path.size() - 1; i++) {
		    int u = path.vertices[i], v = path.vertices[i + 1];
		    auto [bX, edgeX] = graph::checkEdge(x, v, cx, GG);
		    auto [bY, edgeY] = graph::checkEdge(u, y, cy, GG);

		    if (bX && bY) {
			// achamos um crossing
			graph::Path newPath(instance.first);
			for (int j = 0; j <= i; j++) {
			    newPath.push_back(path.vertices[j], j ? path.edges[j - 1] : graph::Edge());
			}
			newPath.push_back(y, edgeY);
			for (int j = path.size() - 1; j >= i + 1; j--) {
			    newPath.push_back(path.vertices[j - 1], path.edges[j - 1]);
			}
			graph::Cycle cycle(newPath, edgeX);
			return cycle;
		    }
		}
	    }
	    throw std::runtime_error("Should not reach here. Did not found crossing.");
	}

// Given a Path
Variant operator()(const graph::Cycle& cycle) {
    const auto& GG = *instance.first;
    const auto& G = instance.second;

    const auto n = boost::num_vertices(GG);
    const uint64_t cycle_size = cycle.size();
    if(cycle_size < 3) {
        throw std::runtime_error("Every cycle must have at least 3 vertices");
    }
    std::vector<bool> used_colors(n);
    int miss1 = 0, miss2 = n - 1;
    while(used_colors[miss1]) miss1++;
    while(used_colors[miss2]) miss2--;
    for(auto& e: cycle.edges)
	    used_colors[GG[e].color] = true;

    if(cycle_size < ceil_div((uint64_t)n, uint64_t(2)) + 1) {
	    auto &x = cycle.vertices;
	    // missed colors
    } else if(cycle_size == n - 1) {
    } else {
    }
}

};

Variant increment(const graph::Instance& instance, const Variant& state) {
	return std::visit(Visitor{instance}, state);
}
