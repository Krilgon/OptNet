#pragma once
#include "Graph.h"
#include <string>

class GraphLoader {
public:
    static Graph loadFromFile(const std::string& path);
};
