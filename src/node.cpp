#include "node.h"
#include <iostream>

std::vector<Node> Node::get_successors(const Sequences &sequences) const {
    return {Node({1, 2}), Node({3, 4})};
}

int Node::compute_cost(const Node &other, const ScoreMatrix &mtx) const {
    return 0;
}

int Node::compute_heuristic(const Sequences &sequences, const ScoreMatrix &mtx) const {
    return 0;
}

std::ostream &operator<<(std::ostream &o, const Node &node) {
    for (int i : node._indices)
        o << i << " ";
    return o;
}


