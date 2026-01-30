#pragma once
#include "Graph.h"
#include <vector>
#include <unordered_map>

namespace PathUtils {
    std::unordered_map<int, std::vector<int>> buildAdj(const Graph& g);

    bool isValidPath(const Graph& g, const std::vector<int>& path);

    // Guaranteed shortest path if exists, else {}
    std::vector<int> bfsPath(const Graph& g);

    // Random walk that tries to reach end; uses adjacency; may fail -> {}
    std::vector<int> randomPath(const Graph& g, int maxLen);

    // Repair a partial path so it ends at end_node if possible
    bool repairToEnd(const Graph& g, std::vector<int>& path);
}
