#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace graph {
  struct BundledVertex {
  };

  struct BundledEdge {
    int color;       // color label
    BundledEdge(int _color = -1) : color(_color) {}
  };

  typedef boost::adjacency_list<boost::vecS,
	  boost::vecS,
	  boost::undirectedS,
	  BundledVertex,
	  BundledEdge> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef boost::graph_traits<Graph>::edge_descriptor Edge;



  struct BundledEdge2 {
    Edge id;          // unique identifier
    BundledEdge2(Edge _id = Edge()) : id(_id) {}
  };
  typedef boost::adjacency_list<boost::vecS,
	  boost::vecS,
	  boost::undirectedS,
	  BundledVertex,
	  BundledEdge2> Graph2;
  typedef boost::graph_traits<Graph2>::vertex_descriptor Vertex2;
  typedef boost::graph_traits<Graph2>::edge_descriptor Edge2;


  using graphPointer = std::shared_ptr<Graph>;

  // TODO: otimizar usando referencia de edges de G[i] para GG
  std::pair<bool, graph::Edge> checkEdge (int u, int v, int color, const Graph& GG) {
      for (const auto& edge : boost::make_iterator_range(boost::out_edges(u, GG))) {
          if (boost::source(edge, GG) == u && boost::target(edge, GG) == v && GG[edge].color == color) {
              return {true, edge};
          }
          if (boost::source(edge, GG) == v && boost::target(edge, GG) == u && GG[edge].color == color) {
              return {true, edge};
          }
      }
      return {false, graph::Edge()};
  };

  struct Path {
    graphPointer G;
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;

    Path (graphPointer _G) : G(_G) {}
    
    int size() const {
      return edges.size();
    }

    Vertex back() const {
      return vertices.back();
    }

    // returns false if the edge does not exist
    // TODO: FIX ADD CORRECT DESCRIPTOR
    bool push_back(Vertex v) {
      if (vertices.size() > 0) {
        Edge e;
        bool b;
        std::tie(e, b) = boost::edge(vertices.back(), v, *G);
        if (not b) {
          return false;
        }
        edges.push_back(e);
      }
      vertices.push_back(v);
      return true;
    }

    void pop_back() {
      assert(vertices.size() > 0 && "Cannot pop from empty path.");
      vertices.pop_back();
      edges.pop_back();
    }
  };

  struct Cycle {
    graphPointer G;
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;

    Cycle (graphPointer _G) : G(_G) {}

    Cycle (const Path& path, Edge edge) : G(path.G) {
      assert(path.vertices.size() == path.edges.size() + 1);
      int x = path.vertices[0], y = path.vertices.back();

      if (target(edge, *G) != x) std::swap(x, y);
      assert(source(edge, *G) == x && "Edge does not belong to the graph.");
      assert(target(edge, *G) == y && "Edge does not belong to the graph.");

      vertices = path.vertices;
      edges = path.edges;
      edges.push_back(edge);
    }

    int size() const {
      return edges.size();
    }
  };

  using Instance = std::pair<graphPointer, std::vector<Graph2>>;
}
