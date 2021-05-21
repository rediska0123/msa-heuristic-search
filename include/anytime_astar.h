#ifndef MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H
#define MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H

#include "input.h"
#include "common.h"

class AnytimeProgressTracker: public ProgressTracker {
public:
    AnytimeProgressTracker(): ProgressTracker(), _bounds({}) {}

    void on_new_iteration(const Open &open, const Closed &closed, int min_bound, int max_bound);

    std::vector<std::pair<int, int>> get_bounds() const;
private:
    std::vector<std::pair<int, int>> _bounds;
};

struct AnytimeAStarSearchResult: public SearchResult {
    AnytimeAStarSearchResult() = default;
    AnytimeAStarSearchResult(const AlignmentOutput &a, const AnytimeProgressTracker &tracker);

    std::pair<int, int> bounds;
};

AnytimeAStarSearchResult AnytimeAStar(const Sequences &sequences, const ScoreMatrix &mtx, int w);

#endif //MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H
