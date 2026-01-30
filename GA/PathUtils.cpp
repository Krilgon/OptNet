#include "PathUtils.h"
#include <queue>
#include <unordered_set>
#include <random>
#include <algorithm>

static std::mt19937 rng(std::random_device{}());

std::unordered_map<int, std::vector<int>> PathUtils::buildAdj(const Graph& g) {
    std::unordered_map<int, std::vector<int>> adj;
    adj.reserve(g.nodes.size());
    for (auto& kv : g.nodes) adj[kv.first] = {};

    for (const auto& e : g.edges) {
        if (!g.nodes.count(e.node_a) || !g.nodes.count(e.node_b)) continue;
        adj[e.node_a].push_back(e.node_b);
        adj[e.node_b].push_back(e.node_a);
    }
    return adj;
}

bool PathUtils::isValidPath(const Graph& g, const std::vector<int>& path) {
    if (path.empty()) return false;
    if (!g.nodes.count(path.front())) return false;
    if (!g.nodes.count(path.back())) return false;

    auto adj = buildAdj(g);
    for (size_t i = 1; i < path.size(); ++i) {
        int a = path[i - 1];
        int b = path[i];
        if (!g.nodes.count(a) || !g.nodes.count(b)) return false;
        const auto& neigh = adj[a];
        if (std::find(neigh.begin(), neigh.end(), b) == neigh.end()) return false;
    }
    return true;
}

static std::vector<int> bfs(const std::unordered_map<int, std::vector<int>>& adj, int start, int goal) {
    if (start == goal) return { start };

    std::queue<int> q;
    std::unordered_map<int, int> parent;
    parent.reserve(adj.size());

    q.push(start);
    parent[start] = start;

    while (!q.empty()) {
        int v = q.front(); q.pop();
        auto it = adj.find(v);
        if (it == adj.end()) continue;

        for (int to : it->second) {
            if (parent.find(to) != parent.end()) continue;
            parent[to] = v;
            if (to == goal) {
                std::vector<int> path;
                int cur = goal;
                while (true) {
                    path.push_back(cur);
                    if (cur == start) break;
                    cur = parent[cur];
                }
                std::reverse(path.begin(), path.end());
                return path;
            }
            q.push(to);
        }
    }
    return {};
}

std::vector<int> PathUtils::bfsPath(const Graph& g) {
    auto adj = buildAdj(g);
    return bfs(adj, g.start_node, g.end_node);
}

std::vector<int> PathUtils::randomPath(const Graph& g, int maxLen) {
    auto adj = buildAdj(g);

    int start = g.start_node;
    int goal = g.end_node;
    if (!adj.count(start) || !adj.count(goal)) return {};

    std::vector<int> path;
    path.reserve((size_t)std::max(4, maxLen));
    path.push_back(start);

    std::unordered_set<int> seen;
    seen.reserve(g.nodes.size());
    seen.insert(start);

    int cur = start;

    for (int step = 0; step < maxLen; ++step) {
        if (cur == goal) return path;

        auto& neigh = adj[cur];
        if (neigh.empty()) break;

        // Shuffle-like pick
        int next = neigh[(size_t)(rng() % neigh.size())];

        // small chance to allow revisits, but usually avoid cycles
        bool allowRevisit = ((rng() % 100) < 10);
        if (!allowRevisit && seen.count(next)) {
            // try few attempts
            bool found = false;
            for (int t = 0; t < 5; ++t) {
                int cand = neigh[(size_t)(rng() % neigh.size())];
                if (!seen.count(cand)) { next = cand; found = true; break; }
            }
            if (!found) {
                // fallback: aim toward goal via BFS from cur
                auto tail = bfs(adj, cur, goal);
                if (!tail.empty() && tail.size() >= 2) {
                    // append tail (skip cur duplicate)
                    for (size_t k = 1; k < tail.size(); ++k) path.push_back(tail[k]);
                    return path;
                }
                // otherwise accept cycle
            }
        }

        path.push_back(next);
        cur = next;
        seen.insert(cur);

        // if reached goal earlier
        if (cur == goal) return path;
    }

    // Final repair attempt
    if (repairToEnd(g, path)) return path;
    return {};
}

bool PathUtils::repairToEnd(const Graph& g, std::vector<int>& path) {
    if (path.empty()) return false;
    auto adj = buildAdj(g);

    int cur = path.back();
    int goal = g.end_node;

    auto tail = bfs(adj, cur, goal);
    if (tail.empty()) return false;
    // tail includes cur as first element
    for (size_t i = 1; i < tail.size(); ++i) path.push_back(tail[i]);
    return true;
}
