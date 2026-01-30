#pragma once
#include <vector>
#include <string>

class JsonExporter {
public:
    static void exportPath(const std::vector<int>& path, double fitness, const std::string& outPath);
};
