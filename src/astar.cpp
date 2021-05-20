#include "astar.h"
#include "heuristic.h"
#include "node.h"
#include "common.h"
#include <algorithm>

SearchResult AStar(const Sequences &sequences, const ScoreMatrix &mtx) {
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);

    std::shared_ptr<Closed> closed = std::make_shared<Closed>();
    std::shared_ptr<Open> open = std::make_shared<Open>(std::shared_ptr<Closed>(closed));

    auto[start_node, goal_node] = get_start_and_goal_nodes(sequences);

    open->add_node(start_node, 0, hc.calculate_heuristic(start_node));
    ProgressTracker tracker;
    while (!open->is_empty()) {
        tracker.on_new_iteration(*open, *closed);
        auto[best_node, g, _] = open->get_best_node();
        closed->add_node(best_node, g);
        if (best_node == goal_node) {
            return SearchResult(path_to_alignment(sequences, get_path(&best_node)), tracker);
        }
        for (const Node &node: best_node.get_successors(sequences)) {
            int h = hc.calculate_heuristic(node);
            int c = best_node.compute_cost(node, sequences, mtx);
            if (!closed->was_expanded(node)) {
                open->add_node(node, g + c, g + c + h);
            }
        }
    }
    return SearchResult(path_to_alignment(sequences, get_path(&goal_node)), tracker);
}