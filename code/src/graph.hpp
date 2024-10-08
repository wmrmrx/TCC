#ifndef _GRAPH_HPP
#define _GRAPH_HPP
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>

namespace graph
{
    struct BundledVertex
    {
    };

    struct BundledEdge
    {
        size_t color; // color label
        BundledEdge();
        BundledEdge(size_t _color);
    };

    typedef boost::adjacency_list<boost::vecS,
                                  boost::vecS,
                                  boost::undirectedS,
                                  BundledVertex,
                                  BundledEdge>
        Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;

    struct BundledEdge2
    {
        EdgeIterator id; // unique identifier
        BundledEdge2();
        BundledEdge2(EdgeIterator _id);
    };
    typedef boost::adjacency_matrix<boost::undirectedS,
                                    BundledVertex,
                                    BundledEdge2>
        Graph2;
    typedef boost::graph_traits<Graph2>::vertex_descriptor Vertex2;
    typedef boost::graph_traits<Graph2>::edge_descriptor Edge2;

    using graphPointer = std::shared_ptr<Graph>;
    using Instance = std::pair<graphPointer, std::vector<Graph2>>;

    size_t color(const Graph &GG, Edge &edge);

    // TODO: otimizar usando referencia de edges de G[i] para GG
    std::pair<bool, graph::Edge> checkEdge(int u, int v, size_t color, const Instance &instance);

    struct Path
    {
        graphPointer G;
        std::vector<Vertex> vertices;
        std::vector<Edge> edges;

        Path(graphPointer _G);
        Path(graphPointer _G, const std::vector<Vertex> &_vertices, const std::vector<Edge> &_edges);

        size_t size() const;

        Vertex back() const;

        // returns false if the edge does not exist
        // TODO: FIX ADD CORRECT DESCRIPTOR
        bool push_back(Vertex v, Edge e);

        void pop_back();
    };

    struct Cycle
    {
        graphPointer G;
        std::vector<Vertex> vertices;
        std::vector<Edge> edges;

        Cycle(graphPointer _G);
        Cycle(graphPointer _G, const std::vector<Vertex> &_vertices, const std::vector<Edge> &_edges);

        size_t size() const;
    };
}
#endif
