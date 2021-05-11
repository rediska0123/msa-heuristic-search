#ifndef MSA_HEURISTIC_SEARCH_COMMON_H
#define MSA_HEURISTIC_SEARCH_COMMON_H

#include "input.h"

typedef std::vector<Sequence> AlignmentOutput;

int calculate_alignment_score(const AlignmentOutput &alignment, const ScoreMatrix &mtx);

#endif //MSA_HEURISTIC_SEARCH_COMMON_H
