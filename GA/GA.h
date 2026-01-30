#pragma once
#include "Graph.h"
#include "Individual.h"
#include <vector>

class GA {
public:
    explicit GA(const Graph& g);

    Individual run();

private:
    const Graph& graph;
    std::vector<Individual> population;
    Individual best;

    void initPopulation();
    void evolve();

    Individual crossover(const Individual& a, const Individual& b);
    void mutate(Individual& ind);

    // helper: evaluate + repair
    bool finalizeCandidate(Individual& ind);
};
