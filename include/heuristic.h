#ifndef MSA_HEURISTIC_SEARCH_HEURISTIC_H
#define MSA_HEURISTIC_SEARCH_HEURISTIC_H

#include "node.h"

class HeuristicCalculator {
public:
    HeuristicCalculator(const Sequences &sequences, const ScoreMatrix &mtx);

    int calculate_heuristic(const Node &node) const;

private:
    std::vector<std::vector<std::vector<std::vector<int>>>> _pair_heuristic;
};

#endif //MSA_HEURISTIC_SEARCH_HEURISTIC_H
