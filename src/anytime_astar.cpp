#include <algorithm>
#include <cassert>
#include <anytime_astar.h>

#include "heuristic.h"
#include "utils.h"
#include "common.h"

AnytimeAStarSearchResult AnytimeAStar(const Sequences &sequences, const ScoreMatrix &mtx, double w, int max_runtime_secs) {
    assert(w >= 1.0);
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);

    NodeStorage storage;
    Closed closed(&storage);
    Open open(&closed, &storage);


    auto[start_node, goal_node] = get_start_and_goal_nodes(sequences);

    open.add_node(start_node, 0, int(w * hc.calculate_heuristic(start_node)));
    Node incumbent({});
    int f_incumbent = INF;

    FValuesStorage st;
    st.update_f_value(start_node, hc.calculate_heuristic(start_node));
    AnytimeProgressTracker tracker(max_runtime_secs);
    while (!open.is_empty()) {
        auto[best_node, g, f] = open.get_best_node();
        if (st.get_f_value(best_node) >= f_incumbent)
            continue;
        closed.add_node(best_node, g);
        for (Node nxt: best_node.get_successors(sequences, storage.get_node_ptr(best_node))) {
            int h = hc.calculate_heuristic(nxt), c = best_node.compute_cost(nxt, sequences, mtx);
            if (g + c + h >= f_incumbent)
                continue;
            if (nxt == goal_node) {
                f_incumbent = g + c;
                st.update_f_value(nxt, g + c);
                incumbent = Node(nxt);
            } else {
                if (closed.was_expanded(nxt) and closed.g_value(nxt) <= g + c)
                    continue;
                if (closed.was_expanded(nxt))
                    closed.delete_node(nxt);
                open.add_node(nxt, g + c, int(w * h + g + c));
                st.update_f_value(nxt, g + c + h);
            }
        }
        if (tracker.on_new_iteration(open, closed, st.get_min_f_value(), f_incumbent))
            return AnytimeAStarSearchResult(path_to_alignment(sequences, get_path(&incumbent)), tracker);
    }

    return AnytimeAStarSearchResult(path_to_alignment(sequences, get_path(&incumbent)), tracker);
}

bool AnytimeProgressTracker::on_new_iteration(const Open &open, const Closed &closed, int min_bound, int max_bound) {
    _bounds.emplace_back(min_bound, max_bound);
    return ProgressTracker::on_new_iteration(open, closed);
}

std::vector<std::pair<int, int>> AnytimeProgressTracker::get_bounds() const {
    return _bounds;
}

AnytimeAStarSearchResult::AnytimeAStarSearchResult(const AlignmentOutput &a, const AnytimeProgressTracker &tracker)
        : SearchResult(a, tracker) {
    bounds = tracker.get_bounds();
}

void FValuesStorage::update_f_value(const Node &n, int new_f) {
    if (_f_value.count(n) && _f_value[n] < new_f)
        return;
    _f_values.erase(_f_value[n]);
    _f_value[n] = new_f;
    _f_values.insert(new_f);
}

int FValuesStorage::get_min_f_value() const {
    if (_f_values.empty())
        return INF;
    return *_f_values.begin();
}

int FValuesStorage::get_f_value(const Node &n) {
    return _f_value[n];
}
