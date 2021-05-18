#include <algorithm>
#include "heuristic_algorithms.h"
#include "heuristic.h"
#include "anytime_astar.h"
#include "utils.h"
#include "common.h"

void ClosedAnytimeAStar::delete_node(const Node &node) {
    _g_values.erase(node);
}

bool ClosedAnytimeAStar::was_expanded(const Node &node) {
    return _g_values.count(node) != 0;
}

int ClosedAnytimeAStar::g_value(const Node &node) {
    return _g_values[node];
}

void ClosedAnytimeAStar::add_node(const Node &node, int g) {
    _g_values[node] = g;
}

std::vector<Node> ClosedAnytimeAStar::get_nodes() {
    std::vector<Node> nodes;
    for (std::pair<Node, int> p : _g_values)
        nodes.push_back(p.first);
    return nodes;
}

SearchResult AnytimeAStar(const Sequences &sequences, const ScoreMatrix &mtx, int w) {
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);
    const size_t N = sequences.size();

    std::shared_ptr<ClosedAnytimeAStar> closed = std::make_shared<ClosedAnytimeAStar>();
    std::shared_ptr<OpenAStar> open = std::make_shared<OpenAStar>(
            std::shared_ptr<ClosedAnytimeAStar>(closed));

    Node start_node = Node(std::vector<int>(N, 0));
    std::vector<int> goal_indices = std::vector<int>(N, 0);
    std::transform(sequences.begin(), sequences.end(), goal_indices.begin(),
                   [](const Sequence &s) { return s.size(); });
    Node goal_node = Node(std::move(goal_indices));

    open->add_node(start_node, 0, w * hc.calculate_heuristic(start_node));
    Node incumbent({});
    int f_incumbent = INF;

    std::unordered_map<Node, int, NodeHashFunction> f_value;
    f_value[start_node] = hc.calculate_heuristic(start_node);
    while (!open->is_empty()) {
        auto[best_node, g] = open->get_best_node();
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
                if (closed->was_expanded(nxt) and closed->g_value(nxt) > g + c)
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
