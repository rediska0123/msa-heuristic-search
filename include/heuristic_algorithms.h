#ifndef MSA_HEURISTIC_SEARCH_HEURISTIC_ALGORITHMS_H
#define MSA_HEURISTIC_SEARCH_HEURISTIC_ALGORITHMS_H

#include "input.h"
#include "node.h"
#include "common.h"
#include <unordered_set>
#include <queue>
#include <memory>

class OpenAStar : public Open {
public:
    explicit OpenAStar(std::shared_ptr<Closed> closed) : _closed(std::move(closed)) {};

    std::pair<Node, int> get_best_node() override;

    void add_node(const Node &node, int g, int f);

    bool is_empty() override;

private:
    struct Comparator {
        constexpr bool operator()(
                std::pair<int, Node> const &a,
                std::pair<int, Node> const &b)
        const noexcept {
            return a.first > b.first;
        }
    };

    std::shared_ptr<Closed> _closed;
    std::priority_queue<std::pair<int, Node>, std::vector<std::pair<int, Node>>, Comparator> _nodes;
    std::unordered_map<Node, int, NodeHashFunction> _g_values;

    void _remove_old_nodes();
};

class ClosedAStar : public Closed {
public:
    bool was_expanded(const Node &node) override;

    void add_node(const Node &node);

private:
    std::unordered_set<Node, NodeHashFunction> _nodes;
};


SearchResult AStar(const Sequences &sequences, const ScoreMatrix &mtx);

#endif //MSA_HEURISTIC_SEARCH_HEURISTIC_ALGORITHMS_H
