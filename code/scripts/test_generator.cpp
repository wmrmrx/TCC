#include <random>
#include <set>
#include <iostream>

std::mt19937 rng(10);

std::vector<std::pair<int, int>> generate_edges(int n)
{
    std::vector<int> degree(n);
    std::set<std::pair<int, int>> used;
    std::vector<std::pair<int, int>> edges;

    while (true) {
        int u = rng() % n;
        int v = rng() % n;
        if (u == v) continue;
        if (u > v) std::swap(u, v);
        if (used.count({u, v})) continue;
        if (degree[u] < (n + 1) / 2 || degree[v] < (n + 1) / 2) {
            degree[u]++; degree[v]++;
            edges.push_back({u, v});
            used.insert({u, v});
        }
        if (*std::min_element(degree.begin(), degree.end()) >= (n + 1) / 2) break;
    }
    return edges;
}

int main(int argc, char* argv[] )
{
    int n = std::stoi(argv[1]);
    std::vector<std::array<int, 3>> colored_edges;

    for (int color = 1; color <= n; color++) {
        auto edges = generate_edges(n);
        for (auto [u, v] : edges) {
            colored_edges.push_back({u, v, color});
        }
    }
    std::cout << n << " " << colored_edges.size() << "\n";
    for (auto [u, v, color] : colored_edges) {
        std::cout << u + 1 << " " << v + 1 << " " << color << "\n";
    }
}