#include "JsonExporter.h"
#include <fstream>
#include <iostream>

static const char* nodeTypeToString(NodeType t) {
    switch (t) {
    case NodeType::PC: return "PC";
    case NodeType::COMPUTE: return "COMPUTE";
    case NodeType::STORAGE: return "STORAGE";
    case NodeType::GATEWAY: return "GATEWAY";
    case NodeType::SERVER: return "SERVER";
    }
    return "UNKNOWN";
}

void JsonExporter::exportGraph(const Graph& g, const std::string& path) {
    std::ofstream out(path);
    if (!out) {
        std::cerr << "[JsonExporter] Failed to write graph\n";
        return;
    }

    out << "{\n";
    out << "\"start_node\": " << g.start_node << ",\n";
    out << "\"end_node\": " << g.end_node << ",\n";

    out << "\"nodes\": [\n";
    bool first = true;
    for (const auto& [id, n] : g.nodes) {
        if (!first) out << ",\n";
        first = false;
        out << "  { \"id\": " << id
            << ", \"type\": \"" << nodeTypeToString(n.type)
            << "\", \"performance\": " << n.performance << " }";
    }
    out << "\n],\n";

    out << "\"edges\": [\n";
    first = true;
    for (const auto& e : g.edges) {
        if (!first) out << ",\n";
        first = false;
        out << "  { \"node_a\": " << e.node_a
            << ", \"node_b\": " << e.node_b
            << ", \"latency\": " << e.latency
            << ", \"bandwidth\": " << e.bandwidth << " }";
    }
    out << "\n]\n}\n";

    std::cout << "[JsonExporter] Graph saved to " << path << "\n";
}
