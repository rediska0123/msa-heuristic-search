#include "heuristic_algorithms.h"
#include "heuristic.h"
#include "node.h"
#include "common.h"
#include <vector>
#include <unordered_map>
#include <algorithm>


std::pair<Node, int> OpenAStar::get_best_node() {
    _remove_old_nodes();
    Node best_node = _nodes.top().second;
    return {best_node, _g_values[best_node]};
}

void OpenAStar::add_node(const Node &node, int g, int f) {
    if (_g_values.find(node) == _g_values.end() || _g_values[node] > g) {
        _nodes.push({f, node});
        _g_values[node] = g;
    }
}

bool OpenAStar::is_empty() {
    _remove_old_nodes();
    return _nodes.empty();
}

void OpenAStar::_remove_old_nodes() {
    while (!_nodes.empty() && _closed->was_expanded(_nodes.top().second)) {
        _nodes.pop();
    }
}

void ClosedAStar::add_node(const Node &node) {
    _nodes.insert(node);
}

bool ClosedAStar::was_expanded(const Node &node) {
    return _nodes.find(node) != _nodes.end();
}

SearchResult AStar(const Sequences &sequences, const ScoreMatrix &mtx) {
    HeuristicCalculator hc = HeuristicCalculator(sequences, mtx);
    const size_t N = sequences.size();

    std::shared_ptr<ClosedAStar> closed = std::make_shared<ClosedAStar>();
    std::shared_ptr<OpenAStar> open = std::make_shared<OpenAStar>(std::shared_ptr<ClosedAStar>(closed));

    Node start_node = Node(std::vector<int>(N, 0));
    std::vector<int> goal_indices = std::vector<int>(N, 0);
    std::transform(sequences.begin(), sequences.end(), goal_indices.begin(),
                   [](const Sequence &s) { return s.size(); });
    Node goal_node = Node(std::move(goal_indices));

    open->add_node(start_node, 0, hc.calculate_heuristic(start_node));
    while (!open->is_empty()) {
        auto[best_node, g] = open->get_best_node();
        closed->add_node(best_node);
        if (best_node == goal_node) {
            return SearchResult{path_to_alignment(sequences, get_path(&best_node)),
                                std::shared_ptr<Open>(open), std::shared_ptr<Closed>(closed)};
        }
        for (const Node &node: best_node.get_successors(sequences)) {
            int h = hc.calculate_heuristic(node);
            int c = best_node.compute_cost(node, sequences, mtx);
            if (!closed->was_expanded(node)) {
                open->add_node(node, g + c, g + c + h);
            }
        }
    }
    return SearchResult{path_to_alignment(sequences, get_path(&goal_node)), std::shared_ptr<Open>(open),
                        std::shared_ptr<Closed>(closed)};
}
