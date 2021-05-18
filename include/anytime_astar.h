#ifndef MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H
#define MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H

#include <utility>

#include "common.h"
#include "input.h"

class ClosedAnytimeAStar : public Closed {
public:
    void add_node(const Node &node, int g);

    void delete_node(const Node &node);

    bool was_expanded(const Node &node) override;

    int g_value(const Node &node);

    std::vector<Node> get_nodes();

private:
    std::unordered_map<Node, int, NodeHashFunction> _g_values;
};

SearchResult AnytimeAStar(const Sequences &sequences, const ScoreMatrix &mtx, int w);

#endif //MSA_HEURISTIC_SEARCH_ANYTIME_ASTAR_H
