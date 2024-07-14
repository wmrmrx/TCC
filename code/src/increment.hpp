#ifndef _INCREMENT_HPP
#define _INCREMENT_HPP
#include "graph.hpp"

// Given a Path
std::variant<graph::Cycle, graph::Path> increment_path(const graph::Instance& instance, const graph::Path& path);
std::variant<graph::Cycle, graph::Path> increment_cycle(const graph::Instance& instance, const graph::Cycle& cycle);
#endif
