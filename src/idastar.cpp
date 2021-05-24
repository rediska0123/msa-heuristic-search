#include "idastar.h"
#include "common.h"
#include "heuristic.h"
#include "utils.h"
#include "cassert"

int search(IDAStarProgressTracker &tracker, const Sequences &seqs, const ScoreMatrix &mtx,
           const HeuristicCalculator &hc, const Node &goal_node, std::vector<Node> &path, int g, int bound) {
    tracker.on_new_iteration(path);
    if (tracker.need_stop())
        return -INF;
    Node n = path.back();
    int f = g + hc.calculate_heuristic(n);
    if (f > bound)
        return f;
    if (n == goal_node)
        return -INF;
    int min_bound = INF;
    for (const Node &nxt: n.get_successors(seqs, nullptr)) {
        // the graph is acyclic, so no need to check whether nxt is in path
        path.push_back(nxt);
        int c = n.compute_cost(nxt, seqs, mtx);
        int t = search(tracker, seqs, mtx, hc, goal_node, path, g + c, bound);
        if (t == -INF)
            return -INF;
        min_bound = std::min(min_bound, t);
        path.pop_back();
    }
    return min_bound;
}

SearchResult IDAStar(const Sequences &sequences, const ScoreMatrix &mtx, int max_runtime_secs) {
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);
    auto[start_node, goal_node] = get_start_and_goal_nodes(sequences);

    int bound = hc.calculate_heuristic(start_node);
    std::vector<Node> path = {start_node};
    IDAStarProgressTracker tracker(max_runtime_secs);
    while (true) {
        bound = search(tracker, sequences, mtx, hc, goal_node, path, 0, bound);
        if (tracker.need_stop())
            return SearchResult(AlignmentOutput(), tracker);
        assert(bound != INF);
        if (bound == -INF)
            return SearchResult(path_to_alignment(sequences, path), tracker);
    }
    assert(false), "No path found";
}

void IDAStarProgressTracker::on_new_iteration(const std::vector<Node> &path) {
    _iterations_num += 1;
    _max_nodes_in_memory = std::max(_max_nodes_in_memory, (int)path.size());
}
