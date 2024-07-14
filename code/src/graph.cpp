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
      if (boost::source(edge, GG) == u && boost::target(edge, GG) == v && GG[edge].color == color)
      {
        return {true, edge};
      }
      if (boost::source(edge, GG) == v && boost::target(edge, GG) == u && GG[edge].color == color)
      {
        return {true, edge};
      }
    }
    return {false, graph::Edge()};
  };

  Path::Path(graphPointer _G) : G(_G) {}

  int Path::size() const
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

  Cycle::Cycle(const Path &path, Edge edge) : G(path.G)
  {
    assert(path.vertices.size() == path.edges.size() + 1);
    int x = path.vertices[0], y = path.vertices.back();

    if (target(edge, *G) != x)
      std::swap(x, y);
    assert(source(edge, *G) == x && "Edge does not belong to the graph.");
    assert(target(edge, *G) == y && "Edge does not belong to the graph.");

    vertices = path.vertices;
    edges = path.edges;
    edges.push_back(edge);
  }

  int Cycle::size() const
  {
    return edges.size();
  }
}
