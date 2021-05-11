#ifndef MSA_HEURISTIC_SEARCH_PROGRESSIVE_ALIGNMENT_H
#define MSA_HEURISTIC_SEARCH_PROGRESSIVE_ALIGNMENT_H

#include "input.h"
#include "common.h"

AlignmentOutput progressive_alignment(const Sequences &sequences, const ScoreMatrix &mtx);

#endif //MSA_HEURISTIC_SEARCH_PROGRESSIVE_ALIGNMENT_H
