#include "idastar.h"
#include "common.h"
#include "heuristic.h"
#include "utils.h"
#include "cassert"

int search(const Sequences &seqs, const ScoreMatrix &mtx, const HeuristicCalculator &hc, const Node &goal_node,
           std::vector<Node> &path, int g, int bound) {
    Node n = path.back();
    int f = g + hc.calculate_heuristic(n);
    if (f > bound)
        return f;
    if (n == goal_node)
        return -INF;
    int min_bound = INF;
    for (const Node &nxt: n.get_successors(seqs)) {
        // the graph is acyclic, so no need to check whether nxt is in path
        path.push_back(nxt);
        int c = n.compute_cost(nxt, seqs, mtx);
        int t = search(seqs, mtx, hc, goal_node, path, g + c, bound);
        if (t == -INF)
            return -INF;
        min_bound = std::min(min_bound, t);
        path.pop_back();
    }
    return min_bound;
}

SearchResult IDAStar(const Sequences &sequences, const ScoreMatrix &mtx) {
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);
    auto[start_node, goal_node] = get_start_and_goal_nodes(sequences);

    int bound = hc.calculate_heuristic(start_node);
    std::vector<Node> path = {start_node};
    while (true) {
        bound = search(sequences, mtx, hc, goal_node, path, 0, bound);
        assert(bound != INF);
        if (bound == -INF)
            return SearchResult{path_to_alignment(sequences, path), std::shared_ptr<Open>(),
                                std::shared_ptr<Closed>()};
    }

    return SearchResult();
}
