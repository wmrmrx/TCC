#include "graph.hpp"

namespace graph
{
  BundledEdge::BundledEdge(int _color) : color(_color) {}

  BundledEdge2::BundledEdge2(Edge _id) : id(_id) {}

  // TODO: otimizar usando referencia de edges de G[i] para GG
  std::pair<bool, graph::Edge> checkEdge(int u, int v, int color, const Graph &GG)
  {
    for (const auto &edge : boost::make_iterator_range(boost::out_edges(u, GG)))
    {
      if (boost::source(edge, GG) == (size_t) u && boost::target(edge, GG) == (size_t) v && GG[edge].color == color)
      {
        return {true, edge};
      }
      if (boost::source(edge, GG) == (size_t) v && boost::target(edge, GG) == (size_t) u && GG[edge].color == color)
      {
        return {true, edge};
      }
    }
    return {false, graph::Edge()};
  };

  Path::Path(graphPointer _G) : G(_G) {}
  Path::Path(graphPointer _G, const std::vector<Vertex> &_vertices, const std::vector<Edge> &_edges) : G(_G), vertices(_vertices), edges(_edges) {
    assert(vertices.size() == edges.size() + 1);
    std::vector<int> usedVertex(G->m_vertices.size(), 0), usedColors(G->m_vertices.size(), 0);
    for (const auto &vertex : vertices) {
      if (usedVertex[vertex] > 0) {
        throw std::runtime_error("Cycle has repeated vertices.");
      }
      usedVertex[vertex]++;
    }
    for (const auto &edge : edges) {
      if (usedColors[(*G)[edge].color] > 0) {
        throw std::runtime_error("Cycle has repeated colors.");
      }
      usedColors[(*G)[edge].color]++;
    }
    for (int i = 0; i + 1 < (int) vertices.size(); i++) {
      bool valid = false;
      if (source(edges[i], *G) != (size_t) vertices[i] 
          && target(edges[i], *G) != (size_t) vertices[(i + 1) % vertices.size()])
        valid = true;
      if (source(edges[i], *G) != (size_t) vertices[(i + 1) % vertices.size()] 
          && target(edges[i], *G) != (size_t) vertices[i])
        valid = true;
      if (!valid) {
        throw std::runtime_error("Cycle has invalid edges.");
      }
    }
  }

  size_t Path::size() const
  {
    return edges.size();
  }

  Vertex Path::back() const
  {
    return vertices.back();
  }

  // returns false if the edge does not exist
  // TODO: FIX ADD CORRECT DESCRIPTOR
  bool Path::push_back(Vertex v, Edge e = Edge())
  {
    if (vertices.size() > 0)
    {
      assert(e != Edge() && "Edge must be provided.");
      edges.push_back(e);
    }
    vertices.push_back(v);
    return true;
  }

  void Path::pop_back()
  {
    assert(vertices.size() > 0 && "Cannot pop from empty path.");
    vertices.pop_back();
    edges.pop_back();
  }

  Cycle::Cycle(graphPointer _G) : G(_G) {}

  Cycle::Cycle(graphPointer _G, const std::vector<Vertex> &_vertices, const std::vector<Edge> &_edges) : G(_G), vertices(_vertices), edges(_edges) {
    assert(vertices.size() == edges.size());
    std::vector<int> usedVertex(G->m_vertices.size(), 0), usedColors(G->m_vertices.size(), 0);
    for (const auto &vertex : vertices) {
      if (usedVertex[vertex] > 0) {
        throw std::runtime_error("Cycle has repeated vertices.");
      }
      usedVertex[vertex]++;
    }
    for (const auto &edge : edges) {
      if (usedColors[(*G)[edge].color] > 0) {
        throw std::runtime_error("Cycle has repeated colors.");
      }
      usedColors[(*G)[edge].color]++;
    }
    for (int i = 0; i < (int) vertices.size(); i++) {
      bool valid = false;
      if (source(edges[i], *G) != (size_t) vertices[i] 
          && target(edges[i], *G) != (size_t) vertices[(i + 1) % vertices.size()])
        valid = true;
      if (source(edges[i], *G) != (size_t) vertices[(i + 1) % vertices.size()] 
          && target(edges[i], *G) != (size_t) vertices[i])
        valid = true;
      if (!valid) {
        throw std::runtime_error("Cycle has invalid edges.");
      }
    }
  }

  Cycle::Cycle(const Path &path, Edge edge) : G(path.G)
  {
    assert(path.vertices.size() == path.edges.size() + 1);
    int x = path.vertices[0], y = path.vertices.back();

    if (target(edge, *G) != (size_t) x)
      std::swap(x, y);
    assert(source(edge, *G) == (size_t) x && "Edge does not belong to the graph.");
    assert(target(edge, *G) == (size_t) y && "Edge does not belong to the graph.");

    vertices = path.vertices;
    edges = path.edges;
    edges.push_back(edge);
  }
  

  size_t Cycle::size() const
  {
    return edges.size();
  }
}
