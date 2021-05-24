#ifndef MSA_HEURISTIC_SEARCH_IDASTAR_H
#define MSA_HEURISTIC_SEARCH_IDASTAR_H

#include "common.h"

SearchResult IDAStar(const Sequences &sequences, const ScoreMatrix &mtx, int max_runtime_secs = 60);

class IDAStarProgressTracker: public ProgressTracker {
public:
    IDAStarProgressTracker(int max_runtime_secs = 60): ProgressTracker(max_runtime_secs) {}
    void on_new_iteration(const std::vector<Node> &path);
};

#endif //MSA_HEURISTIC_SEARCH_IDASTAR_H
