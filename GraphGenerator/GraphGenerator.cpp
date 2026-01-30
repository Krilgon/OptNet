#include "GraphGenerator.h"
#include <random>
#include <vector>
#include <algorithm>
#include <iostream>

static std::mt19937 rng(std::random_device{}());

static int randInt(int a, int b) {
    std::uniform_int_distribution<int> d(a, b);
    return d(rng);
}

static double rand01() {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    return d(rng);
}

static NodeType pickType(int layer, int layers) {
    if (layer == 0) return NodeType::PC;
    if (layer == layers - 1) return NodeType::SERVER;

    double r = rand01();
    if (r < 0.2) return NodeType::STORAGE;
    if (r < 0.4) return NodeType::GATEWAY;
    return NodeType::COMPUTE;
}

Graph GraphGenerator::generate(int n) {
    Graph g;

    const int LAYERS = 6;
    const double INTRA_P = 0.2;
    const double INTER_P = 0.6;

    // ---- nodes ----
    for (int i = 0; i < n; ++i) {
        int layer = (i * LAYERS) / n;

        Node node;
        node.id = i;
        node.type = pickType(layer, LAYERS);
        node.performance = randInt(100, 1000);

        g.nodes[i] = node;

        if (node.type == NodeType::PC && g.start_node == -1)
            g.start_node = i;

        if (node.type == NodeType::SERVER)
            g.end_node = i; // last SERVER becomes end
    }

    // ---- edges ----
    for (int i = 0; i < n; ++i) {
        int layer_i = (i * LAYERS) / n;

        for (int j = i + 1; j < n; ++j) {
            int layer_j = (j * LAYERS) / n;

            bool same = (layer_i == layer_j);
            bool adjacent = (layer_j == layer_i + 1);

            double p = same ? INTRA_P : (adjacent ? INTER_P : 0.0);
            if (rand01() < p) {
                Edge e;
                e.node_a = i;
                e.node_b = j;
                e.latency = rand01() * 10.0 + 1.0;
                e.bandwidth = rand01() * 9.0 + 1.0;
                g.edges.push_back(e);
            }
        }
    }

    // ---- FORCE guaranteed path start -> end ----
    if (g.start_node != -1 && g.end_node != -1 && g.start_node != g.end_node) {
        int current = g.start_node;
        while (current != g.end_node) {
            int next = std::min(current + 1, g.end_node);
            g.edges.push_back({ current, next, 1.0, 10.0 });
            current = next;
        }
    }

    std::cout << "[GraphGenerator] Nodes=" << g.nodes.size()
        << " Edges=" << g.edges.size()
        << " Start=" << g.start_node
        << " End=" << g.end_node << "\n";

    return g;
}

