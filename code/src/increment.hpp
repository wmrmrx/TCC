#ifndef _INCREMENT_HPP
#define _INCREMENT_HPP
#include "graph.hpp"

// Given a Path
std::variant<graph::Cycle, graph::Path> increment(const graph::Instance &instance, const std::variant<graph::Cycle, graph::Path> &state);
#endif
