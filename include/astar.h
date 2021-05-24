#ifndef MSA_HEURISTIC_SEARCH_ASTAR_H
#define MSA_HEURISTIC_SEARCH_ASTAR_H

#include "input.h"
#include "common.h"

SearchResult AStar(const Sequences &sequences, const ScoreMatrix &mtx, int max_runtime_secs = 60);

#endif //MSA_HEURISTIC_SEARCH_ASTAR_H
