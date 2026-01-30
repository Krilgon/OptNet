#pragma once
#include <string>
#include <vector>
#include "Graph.h"

class JsonExporter {
public:
    static void exportGraph(const Graph& g, const std::string& path);
};
