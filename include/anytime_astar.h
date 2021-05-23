#ifndef MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H
#define MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H

#include <set>

#include "input.h"
#include "common.h"

class AnytimeProgressTracker: public ProgressTracker {
public:
    explicit AnytimeProgressTracker(int max_runtime_secs = 60): ProgressTracker(max_runtime_secs), _bounds({}) {}

    bool on_new_iteration(const Open &open, const Closed &closed, int min_bound, int max_bound);

    std::vector<std::pair<int, int>> get_bounds() const;
private:
    std::vector<std::pair<int, int>> _bounds;
};

class FValuesStorage {
public:
    FValuesStorage() = default;

    void update_f_value(const Node &n, int new_f);

    int get_f_value(const Node &n);

    int get_min_f_value() const;

private:
    std::unordered_map<Node, int, NodeHashFunction> _f_value;
    std::multiset<int> _f_values;
};

struct AnytimeAStarSearchResult: public SearchResult {
    AnytimeAStarSearchResult() = default;
    AnytimeAStarSearchResult(const AlignmentOutput &a, const AnytimeProgressTracker &tracker);

    std::vector<std::pair<int, int>> bounds;
};

AnytimeAStarSearchResult AnytimeAStar(const Sequences &sequences, const ScoreMatrix &mtx, double w);

#endif //MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H
