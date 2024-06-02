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
  // for (auto ei = boost::edges(GG); ei.first != ei.second; ++ei.first) {
  //   graph::Edge e = *ei.first;
  //   std::cout << e << "\n";
  //   // std::cout << " " << boost::source(e, GG) << " " << boost::target(e, GG) << " " << GG[e].color << std::endl;
  //   // auto x = boost::source(e, GG), y = boost::target(e, GG), color = GG[e].color;
  //   // graph::Edge a;
  //   // std::tie(a, std::ignore) = boost::add_edge(x, y, G[color]);
  //   // G[color][a].id = GG[e].id;
  //   // G[color][a].color = color;
  // }
  // for (const auto &edge : boost::make_iterator_range(boost::edges(GG))) {
  //   // std::cout << edge << "\n";
  //   auto x = boost::source(edge, GG), y = boost::target(edge, GG), color = GG[edge].color;
  //   graph::Edge a;
  //   std::tie(a, std::ignore) = boost::add_edge(x, y, G[color]);
  //   G[color][a].id = GG[edge].id;
  //   G[color][a].color = color;
  // }
  // for (const auto& edge : boost::edges(GG)) {

    // int x = boost::source(edge, GG), y = boost::target(edge, GG), color = GG[edge].color;
    // // std::tie(e, b) = boost::edge(x, y, GG);
    // graph::Edge e; bool b;
    // std::tie(e, b) = boost::edge(y, x, GG);
    // for (int id = 0; id < boost::num_edges(GG); id++) {
    //   // if (boost::ed)
    // }
    // std::cout << " " << x << " " << y << " " << color << " " << b << " " << e << std::endl;
    // if (not b) {
    //   return false;
    // }
  // }

  return {std::make_shared<graph::Graph>(GG), G};
}

int main(int argc, char** argv)
{
  auto [GG, G] = read_graph(std::cin);

  size_t n = num_vertices(*GG);

  // for (int i = 0; i < n; i++) {
  //   for (const auto& vertex : boost::make_iterator_range(boost::vertices(G[i]))) {
  //     std::cout << "Vertex " << vertex << " has degree " << boost::degree(vertex, G[i]) << std::endl;
  //     for (const auto& edge : boost::make_iterator_range(boost::out_edges(vertex, G[i]))) {
  //       std::cout << "Edge " << G[i][edge].id << " has color " << G[i][edge].color << std::endl;
  //     }
  //   }
  // }
  // for (const auto& vertex : boost::make_iterator_range(boost::vertices(*GG))) {
  //   std::cout << "Vertex " << vertex << " has degree " << boost::degree(vertex, *GG) << std::endl;
  //   for (const auto& edge : boost::make_iterator_range(boost::out_edges(vertex, *GG))) {
  //     std::cout << "Edge " << (*GG)[edge].id << " has color " << (*GG)[edge].color << std::endl;
  //   }
  // }
  
  std::vector<bool> usedColors (n), usedVertex (n), usedEdges (n * (n - 1) / 2);

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