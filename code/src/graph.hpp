#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace graph {
  struct BundledVertex {
  };

  struct BundledEdge {
    int color;       // color label
    int id;          // unique identifier
    BundledEdge(int _color = -1, int _id = -1) : color(_color), id(_id) {}
  };

  typedef boost::adjacency_list<boost::vecS,
	  boost::vecS,
	  boost::undirectedS,
	  BundledVertex,
	  BundledEdge> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef boost::graph_traits<Graph>::edge_descriptor Edge;


  using graphPointer = std::shared_ptr<Graph>;

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
    }
  };

  struct Cycle {
    graphPointer G;
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;

    Cycle (graphPointer _G) : G(_G) {}

    Cycle (const Path& path) {
      assert(path.vertices.size() == path.edges.size() + 1);
      vertices = path.vertices; vertices.pop_back();
      edges = path.edges;
    }

    int size() const {
      return edges.size();
    }
  };

  using Instance = std::pair<graphPointer, std::vector<Graph>>;
}
