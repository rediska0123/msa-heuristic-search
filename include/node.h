#ifndef MSA_HEURISTIC_SEARCH_NODE_H
#define MSA_HEURISTIC_SEARCH_NODE_H

#include "input.h"
#include <utility>
#include <vector>
#include <iostream>

class Node {
public:
    explicit Node(std::vector<int> indices) : _indices(std::move(indices)) {}

    std::vector<Node> get_successors(const Sequences &sequences) const;

    int compute_cost(const Node &other, const ScoreMatrix &mtx) const;

    int compute_heuristic(const Sequences &sequences, const ScoreMatrix &mtx) const;

    friend std::ostream &operator<<(std::ostream &o, const Node &node);

private:
    std::vector<int> _indices;
};

std::ostream &operator<<(std::ostream &o, const Node &node);

#endif //MSA_HEURISTIC_SEARCH_NODE_H

