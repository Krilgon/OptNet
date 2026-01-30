#include "GraphLoader.h"
#include "GA.h"
#include "JsonExporter.h"

#include <iostream>
#include <string>

int main() {
    const std::string inPath = "D:/OptNet/results/input_graph.json";
    const std::string outPath = "D:/OptNet/results/best_path.json";

    try {
        std::cout << "[MAIN] Loading graph from: " << inPath << "\n";
        Graph g = GraphLoader::loadFromFile(inPath);

        std::cout << "[MAIN] Running GA...\n";
        GA ga(g);
        Individual best = ga.run();

        std::cout << "[MAIN] Saving best path to: " << outPath << "\n";
        JsonExporter::exportPath(best.path, best.fitness, outPath);

        std::cout << "[MAIN] Done.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "[MAIN] ERROR: " << e.what() << "\n";
        return 1;
    }
}
