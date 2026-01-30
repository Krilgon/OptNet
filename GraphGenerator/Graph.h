#pragma once
#include <unordered_map>
#include <vector>
#include "Node.h"
#include "Edge.h"

struct Graph {
    std::unordered_map<int, Node> nodes;
    std::vector<Edge> edges;

    int start_node = -1;
    int end_node = -1;
};
