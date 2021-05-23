#include <algorithm>
#include <cassert>
#include <anytime_astar.h>

#include "heuristic.h"
#include "utils.h"
#include "common.h"

AnytimeAStarSearchResult AnytimeAStar(const Sequences &sequences, const ScoreMatrix &mtx, int w) {
    assert(w >= 1);
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);

    NodeStorage storage;
    Closed closed(&storage);
    Open open(&closed, &storage);


    auto[start_node, goal_node] = get_start_and_goal_nodes(sequences);

    open.add_node(start_node, 0, w * hc.calculate_heuristic(start_node));
    Node incumbent({});
    int f_incumbent = INF;

    std::unordered_map<Node, int, NodeHashFunction> f_value;
    f_value[start_node] = hc.calculate_heuristic(start_node);
    AnytimeProgressTracker tracker;
    while (!open.is_empty()) {
        auto[best_node, g, f] = open.get_best_node();
        if (f_value[best_node] >= f_incumbent)
            continue;
        tracker.on_new_iteration(open, closed, f, f_incumbent);
        closed.add_node(best_node, g);
        for (Node nxt: best_node.get_successors(sequences, storage.get_node_ptr(best_node))) {
            int h = hc.calculate_heuristic(nxt), c = best_node.compute_cost(nxt, sequences, mtx);
            if (g + c + h >= f_incumbent)
                continue;
            if (nxt == goal_node) {
                f_incumbent = g + c;
                f_value[nxt] = g + c;
                incumbent = Node(nxt);
            } else {
                if (closed.was_expanded(nxt) and closed.g_value(nxt) <= g + c)
                    continue;
                if (closed.was_expanded(nxt))
                    closed.delete_node(nxt);
                open.add_node(nxt, g + c, g + c + w * h);
                f_value[nxt] = f_value.count(nxt) != 0 ? std::min(f_value[nxt], g + c + h) : g + c + h;
            }
        }
    }

    return AnytimeAStarSearchResult(path_to_alignment(sequences, get_path(&incumbent)), tracker);
}

void AnytimeProgressTracker::on_new_iteration(const Open &open, const Closed &closed, int min_bound, int max_bound) {
    ProgressTracker::on_new_iteration(open, closed);
    _bounds.emplace_back(min_bound, max_bound);
}

std::vector<std::pair<int, int>> AnytimeProgressTracker::get_bounds() const {
    return _bounds;
}

AnytimeAStarSearchResult::AnytimeAStarSearchResult(const AlignmentOutput &a, const AnytimeProgressTracker &tracker)
        : SearchResult(a, tracker) {
    bounds = tracker.get_bounds();
}
