#pragma once
#include <vector>

struct Individual {
    std::vector<int> path;
    double fitness = 1e18;
};
