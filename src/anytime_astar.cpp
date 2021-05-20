#include <algorithm>
#include <cassert>
#include "heuristic.h"
#include "utils.h"
#include "common.h"

SearchResult AnytimeAStar(const Sequences &sequences, const ScoreMatrix &mtx, int w) {
    assert(w >= 1);
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);

    std::shared_ptr<Closed> closed = std::make_shared<Closed>();
    std::shared_ptr<Open> open = std::make_shared<Open>(
            std::shared_ptr<Closed>(closed));

    auto[start_node, goal_node] = get_start_and_goal_nodes(sequences);

    open->add_node(start_node, 0, w * hc.calculate_heuristic(start_node));
    Node incumbent({});
    int f_incumbent = INF;

    std::unordered_map<Node, int, NodeHashFunction> f_value;
    f_value[start_node] = hc.calculate_heuristic(start_node);
    while (!open->is_empty()) {
        auto[best_node, g, _] = open->get_best_node();
        closed->add_node(best_node, g);
        if (f_value[best_node] >= f_incumbent)
            continue;
        for (Node nxt: best_node.get_successors(sequences)) {
            int h = hc.calculate_heuristic(nxt), c = best_node.compute_cost(nxt, sequences, mtx);
            if (g + c + h >= f_incumbent)
                continue;
            nxt.set_parent(std::make_shared<Node>(best_node));
            if (nxt == goal_node) {
                f_incumbent = g + c;
                f_value[nxt] = g + c;
                incumbent = Node(nxt);
            } else {
                if (closed->was_expanded(nxt) and closed->g_value(nxt) <= g + c)
                    continue;
                if (closed->was_expanded(nxt))
                    closed->delete_node(nxt);
                open->add_node(nxt, g + c, g + c + w * h);
                f_value[nxt] = f_value.count(nxt) != 0 ? std::min(f_value[nxt], g + c + h) : g + c + h;
            }
        }
    }

    return SearchResult{path_to_alignment(sequences, get_path(&incumbent)),
                        std::shared_ptr<Open>(open),
                        std::shared_ptr<Closed>(closed)};
}
