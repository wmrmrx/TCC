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

std::pair<graph::Graph, std::vector<graph::Graph>> read_graph(std::istream& is)
{
  typename boost::graph_traits<graph::Graph>::vertices_size_type n; is >> n;

  graph::Graph GG (n);
  std::vector<graph::Graph> G (n, graph::Graph(n));

  size_t m; is >> m;

  for (int i = 0; i < m; i++) {
    int u, v, color; is >> u >> v >> color;
    u--; v--; color--;
    graph::Edge a;
    std::tie(a, std::ignore) = boost::add_edge(u, v, GG);
    GG[a].color = color;
    GG[a].id = i;

    Edge b;
    std::tie(b, std::ignore) = boost::add_edge(u, v, G[color]);
    G[color][b].id = i;
  }

  for (int i = 0; i < n; i++) {
    for (const auto& vertex : boost::make_iterator_range(boost::vertices(G[i]))) {
      if (boost::degree(vertex, G[i]) < (uint64_t) ceil_div(num_vertices(G[i]), 2)) {
        throw std::runtime_error("Input graph is not valid.");
      }
    }
  }

  return {GG, G};
}

int main(int argc, char** argv)
{
  auto [GG, G] = read_graph(std::cin);

  size_t n = num_vertices(GG);

  for (int i = 0; i < n; i++) {
    for (const auto& vertex : boost::make_iterator_range(boost::vertices(G[i]))) {
      std::cout << "Vertex " << vertex << " has degree " << boost::degree(vertex, G[i]) << std::endl;
      for (const auto& edge : boost::make_iterator_range(boost::out_edges(vertex, G[i]))) {
        std::cout << "Edge " << G[i][edge].id << " has color " << G[i][edge].color << std::endl;
      }
    }
  }
  
  std::vector<bool> usedColors (n), usedVertex (n);

  // We can greedily increase the path until it has size greather than n / 2
  // due to degree constraints: deg(v) >= n / 2 for all v
  // Path path;

  // while (path.size() <= n / 2) {
  //   auto u = path.back();
  //   int c = std::find(usedColors.begin(), usedColors.end(), false) - usedColors.begin();
  //   for (const auto &v : boost::make_iterator_range(boost::adjacent_vertices(u, G[c]))) {
  //       if(not usedVertex[v]) {
  //           usedColors[c] = true;
  //           usedVertex[v] = true;
  //           path.push_back(v);
  //           break;
  //       }
  //   }
  //   if(not usedColors[c]) {
  //       throw std::runtime_error("?");
  //   }
  // }

  return EXIT_SUCCESS;
}
