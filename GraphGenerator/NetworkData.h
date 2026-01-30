#pragma once
#include <unordered_map>
#include "Node.h"
#include "Edge.h"

struct NetworkData {
    std::unordered_map<int, Node> nodes;
    std::vector<Edge> edges;
};
