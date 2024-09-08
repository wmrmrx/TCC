#include <cstdlib> // for size_t, EXIT_SUCCESS, and EXIT_FAILURE

#include <iostream>
#include <tuple> // for std::tie and std::ignore
#include <vector>
#include <numeric>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/optional.hpp>

#include "boost/graph/detail/adjacency_list.hpp"
#include "graph.hpp"
#include "util.hpp"

#include "increment.hpp"

using boost::num_vertices;

graph::Instance read_graph(std::istream &is)
{
    typename boost::graph_traits<graph::Graph>::vertices_size_type n;
    is >> n;

    graph::Graph GG(n);
    std::vector<graph::Graph2> G(n, graph::Graph2(n));

    size_t m;
    is >> m;

    for (size_t i = 0; i < m; i++)
    {
        int u, v, color;
        is >> u >> v >> color;
        u--;
        v--;
        color--;
        auto [a, _1] = boost::add_edge(u, v, GG);
        GG[a].color = color;
    }
    auto [beg, end] =  boost::edges(GG);
    for(auto it = beg; it != end; it++) {
        size_t u = boost::source(*it, GG), v = boost::target(*it, GG), color = GG[*it].color;

        auto [b, _] = boost::add_edge(u, v, G[color]);
        G[color][b].id = it;
    }

    for (size_t i = 0; i < n; i++)
    {
        for (const auto &vertex : boost::make_iterator_range(boost::vertices(G[i])))
        {
            if (boost::out_degree(vertex, G[i]) < ceil_div(num_vertices(G[i]), 2ull))
            {
                throw std::runtime_error("Input graph is not valid.");
            }
        }
    }

    return {std::make_shared<graph::Graph>(GG), G};
}

graph::Cycle brute_force(const graph::Instance &instance)
{
    auto [GG, G] = instance;
    size_t n = num_vertices(*GG);

    std::vector<graph::Vertex> vertices(n);
    std::iota(vertices.begin(), vertices.end(), 0);
    do
    {
        std::vector<int> colors(n);
        std::iota(colors.begin(), colors.end(), 0);
        do
        {
            std::vector<graph::Edge> edges;
            for (size_t i = 0; i < n; i++)
            {
                size_t u = vertices[i], v = vertices[(i + 1) % n];
                size_t color = colors[i];
                auto [exists, edge] = graph::checkEdge(u, v, color, instance);
                if (!exists)
                    break;
                edges.push_back(edge);
            }
            if (edges.size() == n)
            {
                return graph::Cycle(GG, vertices, edges);
            }
        } while (std::next_permutation(colors.begin(), colors.end()));
    } while (std::next_permutation(vertices.begin(), vertices.end()));
    throw std::runtime_error("No cycle found. Small test case.");
}

void print_cycle(const graph::Cycle &cycle)
{
    std::cout << "CYCLE SIZE: " << cycle.size() << '\n';
    for (size_t i = 0; i < cycle.size(); i++)
    {
        std::cout << cycle.vertices[i] + 1 << ' ' << cycle.vertices[(i + 1) % cycle.size()] + 1 << ' ' << cycle.edges[i] << ' ' << (*cycle.G)[cycle.edges[i]].color << std::endl;
    }
}

void print_path(const graph::Path &path)
{
    std::cout << "PATH SIZE: " << path.size() << '\n';
    for (size_t i = 0; i < path.size(); i++)
    {
        std::cout << path.vertices[i] + 1 << ' ' << path.vertices[i + 1] + 1 << ' ' << path.edges[i] << ' ' << (*path.G)[path.edges[i]].color << std::endl;
    }
}

void print_object(std::variant<graph::Cycle, graph::Path> object)
{
    if (std::holds_alternative<graph::Cycle>(object))
    {
        auto cycle = std::get<graph::Cycle>(object);
        print_cycle(cycle);
    }
    else
    {
        auto path = std::get<graph::Path>(object);
        print_path(path);
    }
}

int main()
{
    auto instance = read_graph(std::cin);
    auto [GG, G] = instance;

    size_t n = num_vertices(*GG);

    auto [edge, exists] = graph::checkEdge(0, 1, 1, instance);

    // if (n <= 6) {
    //   auto cycle = brute_force({GG, G});
    //   print_cycle(cycle);
    // }
    // else {
    //   graph::Path path(GG);
    //   path.push_back(0, graph::Edge());
    //   auto object = increment({GG, G}, path);
    //   while (true) {
    //     if (std::holds_alternative<graph::Cycle>(object)) {
    //       auto cycle = std::get<graph::Cycle>(object);
    //       if (cycle.size() == n) break;
    //     }
    //     object = increment({GG, G}, object);
    //   }
    //   print_object(object);
    // }

    return EXIT_SUCCESS;
}
