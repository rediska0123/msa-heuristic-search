#ifndef MSA_HEURISTIC_SEARCH_IDASTAR_H
#define MSA_HEURISTIC_SEARCH_IDASTAR_H

#include "common.h"

SearchResult IDAStar(const Sequences &sequences, const ScoreMatrix &mtx);

class IDAStarProgressTracker: public ProgressTracker {
public:
    void on_new_iteration(const std::vector<Node> &path);
};

#endif //MSA_HEURISTIC_SEARCH_IDASTAR_H
