#include "Fitness.h"
#include "PathUtils.h"
#include <limits>
#include <unordered_map>
#include <cmath>

static const Edge* findEdge(const Graph& g, int a, int b) {
    for (const auto& e : g.edges) {
        if ((e.node_a == a && e.node_b == b) || (e.node_a == b && e.node_b == a)) return &e;
    }
    return nullptr;
}

double Fitness::evaluate(const Graph& g, const std::vector<int>& path) {
    if (path.empty()) return 1e18;
    if (path.front() != g.start_node) return 1e18;
    if (path.back() != g.end_node) return 1e18;
    if (!PathUtils::isValidPath(g, path)) return 1e18;

    // Factors:
    // 1) total latency
    // 2) hop count
    // 3) bandwidth penalty (prefer bigger bandwidth)
    // 4) performance bonus (prefer higher node performance along path)
    // 5) loop penalty (avoid revisits)
    double totalLatency = 0.0;
    double invBandwidthSum = 0.0;
    double perfSum = 0.0;

    std::unordered_map<int, int> visits;
    visits.reserve(path.size());

    for (int v : path) {
        visits[v]++;
        auto it = g.nodes.find(v);
        if (it != g.nodes.end()) perfSum += (double)it->second.performance;
    }

    for (size_t i = 1; i < path.size(); ++i) {
        const Edge* e = findEdge(g, path[i - 1], path[i]);
        if (!e) return 1e18;

        double lat = std::max(0.001, e->latency);
        double bw = std::max(0.001, e->bandwidth);

        totalLatency += lat;
        invBandwidthSum += (lat / bw); // penalty grows if bw small
    }

    double hops = (double)(path.size() - 1);

    // loop penalty
    double loopPenalty = 0.0;
    for (auto& kv : visits) {
        if (kv.second > 1) loopPenalty += (kv.second - 1) * 50.0;
    }

    // Normalize performance into a "bonus" (we subtract it)
    // keep it bounded so it doesn't dominate
    double perfAvg = perfSum / std::max<size_t>(1, path.size());
    double perfBonus = std::log1p(std::max(0.0, perfAvg)) * 2.0;

    // Weights (tunable)
    const double W_LAT = 1.0;
    const double W_HOPS = 2.5;
    const double W_BW = 25.0;
    const double W_LOOP = 1.0;
    const double W_PERF = 1.0;

    double score =
        W_LAT * totalLatency +
        W_HOPS * hops +
        W_BW * invBandwidthSum +
        W_LOOP * loopPenalty -
        W_PERF * perfBonus;

    return score;
}
