#include "GA.h"
#include "PathUtils.h"
#include "Fitness.h"

#include <iostream>
#include <random>
#include <algorithm>

static std::mt19937 rng(std::random_device{}());

static constexpr int POP_SIZE = 120;
static constexpr int GENERATIONS = 80;

static constexpr int MAX_INIT_TRIES = 5000;
static constexpr int MAX_EVOLVE_TRIES = 8000;

static constexpr int MAX_RANDOM_LEN = 60;

GA::GA(const Graph& g) : graph(g) {}

bool GA::finalizeCandidate(Individual& ind) {
    if (ind.path.empty()) return false;

    // must start from graph.start_node
    if (ind.path.front() != graph.start_node) {
        // attempt to prefix with bfs from start to first
        Graph tmp = graph; // not used; just for api symmetry
        (void)tmp;
        auto adj = PathUtils::buildAdj(graph);
        // BFS from start to current first
        // reuse internal BFS by calling bfsPath on temporary trick:
        // easiest: just fail and regenerate
        return false;
    }

    // ensure ends at end_node
    if (ind.path.back() != graph.end_node) {
        if (!PathUtils::repairToEnd(graph, ind.path)) return false;
    }

    if (!PathUtils::isValidPath(graph, ind.path)) return false;

    ind.fitness = Fitness::evaluate(graph, ind.path);
    if (ind.fitness >= 1e17) return false;
    return true;
}

Individual GA::run() {
    std::cout << "[GA] Starting genetic algorithm...\n";
    std::cout << "[GA] Start=" << graph.start_node << " End=" << graph.end_node << "\n";

    // Quick check: does any path exist at all?
    auto bfs = PathUtils::bfsPath(graph);
    if (bfs.empty()) {
        std::cout << "[GA] ERROR: No path exists between start and end (BFS failed).\n";
        Individual fail;
        fail.path = { graph.start_node };
        fail.fitness = 1e18;
        return fail;
    }

    initPopulation();
    best = population.front();

    for (int gen = 0; gen < GENERATIONS; ++gen) {
        evolve();

        for (const auto& ind : population) {
            if (ind.fitness < best.fitness) best = ind;
        }

        std::cout << "[GEN " << gen << "] Best fitness: " << best.fitness
            << " | Path len: " << best.path.size() << "\n";
    }

    return best;
}

void GA::initPopulation() {
    population.clear();
    population.reserve(POP_SIZE);

    // Always seed with BFS shortest path (guaranteed baseline)
    {
        auto p = PathUtils::bfsPath(graph);
        Individual ind;
        ind.path = p;
        ind.fitness = Fitness::evaluate(graph, p);
        population.push_back(ind);
    }

    int tries = 0;
    while ((int)population.size() < POP_SIZE && tries < MAX_INIT_TRIES) {
        ++tries;

        auto path = PathUtils::randomPath(graph, MAX_RANDOM_LEN);
        if (path.empty()) continue;

        Individual ind;
        ind.path = std::move(path);
        if (!finalizeCandidate(ind)) continue;

        population.push_back(ind);
    }

    // If still too small — duplicate best-ish
    while ((int)population.size() < POP_SIZE) {
        population.push_back(population[rng() % population.size()]);
    }

    // sort by fitness
    std::sort(population.begin(), population.end(),
        [](const Individual& a, const Individual& b) { return a.fitness < b.fitness; });

    std::cout << "[GA] Initial population: " << population.size()
        << " | Best seed fitness: " << population.front().fitness << "\n";
}

static const Individual& tournamentPick(const std::vector<Individual>& pop, int k) {
    const Individual* best = nullptr;
    for (int i = 0; i < k; ++i) {
        const Individual& cand = pop[rng() % pop.size()];
        if (!best || cand.fitness < best->fitness) best = &cand;
    }
    return *best;
}

void GA::evolve() {
    std::vector<Individual> next;
    next.reserve(POP_SIZE);

    // Elitism: keep top N
    const int ELITE = 8;
    for (int i = 0; i < ELITE && i < (int)population.size(); ++i) {
        next.push_back(population[i]);
    }

    int tries = 0;
    while ((int)next.size() < POP_SIZE && tries < MAX_EVOLVE_TRIES) {
        ++tries;

        const Individual& a = tournamentPick(population, 5);
        const Individual& b = tournamentPick(population, 5);

        Individual child = crossover(a, b);
        mutate(child);

        if (!finalizeCandidate(child)) continue;
        next.push_back(std::move(child));
    }

    // fill if needed
    while ((int)next.size() < POP_SIZE) {
        next.push_back(population[rng() % population.size()]);
    }

    // re-sort
    std::sort(next.begin(), next.end(),
        [](const Individual& x, const Individual& y) { return x.fitness < y.fitness; });

    population = std::move(next);
}

// Crossover: splice at a common node (more valid than random cut)
// If no common node -> take prefix of A then repair to end.
Individual GA::crossover(const Individual& a, const Individual& b) {
    Individual child;

    if (a.path.size() < 2) return a;
    if (b.path.size() < 2) return a;

    // Collect common nodes excluding endpoints
    std::vector<int> common;
    common.reserve(16);

    for (size_t i = 1; i + 1 < a.path.size(); ++i) {
        int v = a.path[i];
        // quick search in b
        if (std::find(b.path.begin() + 1, b.path.end() - 1, v) != (b.path.end() - 1)) {
            common.push_back(v);
        }
    }

    if (!common.empty()) {
        int pivot = common[rng() % common.size()];

        // cut A up to pivot
        auto itA = std::find(a.path.begin(), a.path.end(), pivot);
        auto itB = std::find(b.path.begin(), b.path.end(), pivot);

        child.path.insert(child.path.end(), a.path.begin(), itA + 1);
        child.path.insert(child.path.end(), itB + 1, b.path.end());
    }
    else {
        // fallback: take a prefix of A
        size_t cut = 1 + (rng() % (a.path.size() - 1));
        child.path.insert(child.path.end(), a.path.begin(), a.path.begin() + cut);
    }

    // ensure start is correct
    if (!child.path.empty()) child.path.front() = graph.start_node;

    return child;
}

void GA::mutate(Individual& ind) {
    if (ind.path.size() < 3) return;

    // Mutate with probability
    if ((rng() % 100) >= 35) return;

    // Strategy A: replace tail from a random cut with a new random segment
    if ((rng() % 100) < 70) {
        size_t cut = 1 + (rng() % (ind.path.size() - 2));
        std::vector<int> prefix(ind.path.begin(), ind.path.begin() + cut);

        // Temporarily set end to same graph end; randomPath always starts at graph.start_node,
        // so we create a "local" patch: repair from prefix.back to end via BFS/random.
        // easiest: just repair using BFS from last node.
        ind.path = std::move(prefix);
        PathUtils::repairToEnd(graph, ind.path);
        return;
    }

    // Strategy B: small shuffle inside path (swap two inner nodes) then repair
    size_t i = 1 + (rng() % (ind.path.size() - 2));
    size_t j = 1 + (rng() % (ind.path.size() - 2));
    if (i != j) std::swap(ind.path[i], ind.path[j]);

    // remove obvious invalid adjacency by repairing from earliest break
    if (!PathUtils::isValidPath(graph, ind.path)) {
        // keep prefix until first invalid edge
        auto adj = PathUtils::buildAdj(graph);
        std::vector<int> fixed;
        fixed.push_back(ind.path.front());

        for (size_t k = 1; k < ind.path.size(); ++k) {
            int prev = fixed.back();
            int cur = ind.path[k];
            auto it = adj.find(prev);
            if (it == adj.end()) break;
            auto& neigh = it->second;
            if (std::find(neigh.begin(), neigh.end(), cur) == neigh.end()) break;
            fixed.push_back(cur);
        }
        ind.path = std::move(fixed);
        PathUtils::repairToEnd(graph, ind.path);
    }
}
