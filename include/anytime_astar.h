#ifndef MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H
#define MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H

#include "input.h"
#include "common.h"

SearchResult AnytimeAStar(const Sequences &sequences, const ScoreMatrix &mtx, int w);

#endif //MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H
