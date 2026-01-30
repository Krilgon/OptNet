#include "JsonExporter.h"
#include <fstream>
#include <iostream>

void JsonExporter::exportPath(const std::vector<int>& path, double fitness, const std::string& outPath) {
    std::ofstream out(outPath, std::ios::binary);
    if (!out) {
        std::cerr << "[JsonExporter] Failed to write best path: " << outPath << "\n";
        return;
    }

    out << "{\n";
    out << "  \"fitness\": " << fitness << ",\n";
    out << "  \"path\": [";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i) out << ", ";
        out << path[i];
    }
    out << "]\n";
    out << "}\n";

    std::cout << "[JsonExporter] Best path saved to " << outPath << "\n";
}
