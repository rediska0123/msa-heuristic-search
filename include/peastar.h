#ifndef MSA_HEURISTIC_SEARCH_PEASTAR_H
#define MSA_HEURISTIC_SEARCH_PEASTAR_H

#include "heuristic.h"
#include "common.h"

SearchResult PEAStar(const Sequences &sequences, const ScoreMatrix &mtx, int C);

#endif //MSA_HEURISTIC_SEARCH_PEASTAR_H
