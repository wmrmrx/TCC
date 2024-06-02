#include <cstdlib>              // for size_t, EXIT_SUCCESS, and EXIT_FAILURE

#include <iostream>
#include <tuple>                // for std::tie and std::ignore
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/optional.hpp>

#include "boost/graph/detail/adjacency_list.hpp"
#include "graph.hpp"

using boost::num_vertices;

int ceil_div(int a, int b) {
    return (a + b - 1) / b;
}

graph::Instance read_graph(std::istream& is)
{
  typename boost::graph_traits<graph::Graph>::vertices_size_type n; is >> n;

  graph::Graph GG (n);
  std::vector<graph::Graph2> G (n, graph::Graph2(n));

  size_t m; is >> m;

  for (int i = 0; i < m; i++) {
    int u, v, color; is >> u >> v >> color;
    u--; v--; color--;
    graph::Edge a;
    std::tie(a, std::ignore) = boost::add_edge(u, v, GG);
    GG[a].color = color;

    graph::Edge b;
    std::tie(b, std::ignore) = boost::add_edge(u, v, G[color]);
    G[color][b].id = a;
  }

  for (int i = 0; i < n; i++) {
    for (const auto& vertex : boost::make_iterator_range(boost::vertices(G[i]))) {
      if (boost::degree(vertex, G[i]) < (uint64_t) ceil_div(num_vertices(G[i]), 2)) {
        throw std::runtime_error("Input graph is not valid.");
      }
    }
  }

  return {std::make_shared<graph::Graph>(GG), G};
}

int main(int argc, char** argv)
{
  auto [GG, G] = read_graph(std::cin);

  size_t n = num_vertices(*GG);

  std::vector<bool> usedColors (n), usedVertex (n), usedEdges (n * (n - 1) / 2);

  return EXIT_SUCCESS;
}
