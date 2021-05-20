#include "peastar.h"
#include <algorithm>

SearchResult PEAStar(const Sequences &sequences, const ScoreMatrix &mtx, int C) {
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);

    std::shared_ptr<Closed> closed = std::make_shared<Closed>();
    std::shared_ptr<Open> open = std::make_shared<Open>(std::shared_ptr<Closed>(closed));

    auto[start_node, goal_node] = get_start_and_goal_nodes(sequences);

    open->add_node(start_node, 0, hc.calculate_heuristic(start_node));
    ProgressTracker tracker;
    while (!open->is_empty()) {
        tracker.on_new_iteration(*open, *closed);
        auto[best_node, g, F] = open->get_best_node();
        if (best_node == goal_node) {
            return SearchResult(path_to_alignment(sequences, get_path(&best_node)), tracker);
        }
        std::vector<int> unpromising_node_values;
        for (const Node &node: best_node.get_successors(sequences)) {
            int h = hc.calculate_heuristic(node);
            int c = best_node.compute_cost(node, sequences, mtx);
            int f = g + c + h;
            if (f <= F + C) {
                if (closed->was_expanded(node)) {
                    if (g + c < closed->g_value(node)) {
                        closed->delete_node(node);
                        open->add_node(node, g + c, f);
                    }
                } else {
                    open->add_node(node, g + c, f);
                }
            } else {
                unpromising_node_values.push_back(f);
            }
        }
        if (unpromising_node_values.empty()) {
            closed->add_node(best_node, g);
        } else {
            int new_F = *std::min_element(unpromising_node_values.begin(), unpromising_node_values.end());
            open->add_node(best_node, g, new_F);
        }
    }
    return SearchResult(path_to_alignment(sequences, get_path(&goal_node)), tracker);
}