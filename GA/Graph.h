#pragma once
#include <unordered_map>
#include <vector>
#include <string>

enum class NodeType {
    PC,
    COMPUTE,
    SERVER,
    STORAGE,
    GATEWAY,
    UNKNOWN
};

struct Node {
    int id = 0;
    int performance = 0;
    NodeType type = NodeType::UNKNOWN;
};

struct Edge {
    int node_a = 0;
    int node_b = 0;
    double latency = 1.0;     // ms-ish
    double bandwidth = 1.0;   // arbitrary units
};

struct Graph {
    std::unordered_map<int, Node> nodes;
    std::vector<Edge> edges;

    // for GA
    int start_node = 0;
    int end_node = 0;
};
