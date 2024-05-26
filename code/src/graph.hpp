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

	/* the remainder of the file must not be changed */
	typedef boost::adjacency_list<boost::vecS,
				      boost::vecS,
				      boost::undirectedS,
				      BundledVertex,
				      BundledEdge> Graph;
	typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<Graph>::edge_descriptor Edge;
}
