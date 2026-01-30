#include "GraphGenerator.h"
#include "JsonExporter.h"
#include <iostream>

int main() {
    const std::string outPath = "D:/OptNet/results/input_graph.json";

    GraphGenerator gen;
    Graph g = gen.generate(40);

    JsonExporter::exportGraph(g, outPath);

    return 0;
}
