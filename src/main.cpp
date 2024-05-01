#include <cstdlib>              // for size_t, EXIT_SUCCESS, and EXIT_FAILURE

#include <exception>
#include <iostream>
#include <tuple>                // for std::tie and std::ignore
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/optional.hpp>

#include "graph.h"

using boost::optional;
using boost::num_vertices;

using std::cout;
using std::endl;

int ceil_div(int a, int b) {
    return (a + b - 1) / b;
}

std::pair<Graph, std::vector<Graph>> read_graph(std::istream& is)
{
  typename boost::graph_traits<Graph>::vertices_size_type n; is >> n;

  Graph GG (n);
  std::vector<Graph> G (n, Graph(n));

  size_t m; is >> m;

  while (m--) {
    int u, v, color; is >> u >> v >> color;
    u--; v--; color--;
    Edge a;
    std::tie(a, std::ignore) = boost::add_edge(u, v, GG);
    boost::add_edge(u, v, G[color]);
    GG[a].color = color;
  }

  for (int i = 0; i < n; i++) {
    for (const auto& vertex : boost::make_iterator_range(boost::vertices(G[i]))) {
      if (boost::degree(vertex, G[i]) < ceil_div(num_vertices(G[i]), 2)) {
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
  
  std::vector<bool> usedColors (n), usedVertex (n);

  // We can greedily increase the path until it has size greather than n / 2
  // due to degree constraints: deg(v) >= n / 2 for all v
  std::vector<Vertex> path = {0};
  usedVertex[0] = true;

  while (path.size() <= n / 2) {
    auto u = path.back();
    int c = std::find(usedColors.begin(), usedColors.end(), false) - usedColors.begin();
    for (const auto &v : boost::make_iterator_range(boost::adjacent_vertices(u, G[c]))) {
        if(not usedVertex[v]) {
            usedColors[c] = true;
            usedVertex[v] = true;
            path.push_back(v);
            break;
        }
    }
    if(not usedColors[c]) {
        throw std::runtime_error("?");
    }
  }


  return EXIT_SUCCESS;
}
