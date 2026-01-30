#pragma once
#include "Graph.h"
#include <vector>

class Fitness {
public:
    // Lower is better
    static double evaluate(const Graph& g, const std::vector<int>& path);
};
