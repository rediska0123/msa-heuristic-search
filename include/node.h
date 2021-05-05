#ifndef MSA_HEURISTIC_SEARCH_NODE_H
#define MSA_HEURISTIC_SEARCH_NODE_H

#include "input.h"
#include <utility>
#include <vector>
#include <iostream>
#include <unordered_set>

class Node;

class NodeHashFunction;

class Node {
public:
    explicit Node(std::vector<int> indices) : _indices(std::move(indices)) {}

    std::unordered_set<Node, NodeHashFunction> get_successors(const Sequences &sequences) const;

    int compute_cost(const Node &other, const Sequences &sequences, const ScoreMatrix &mtx) const;

    friend std::ostream &operator<<(std::ostream &o, const Node &node);

    friend NodeHashFunction;

    bool operator==(const Node &other) const;

    int operator[](int index) const;

    size_t size() const;

private:
    const std::vector<int> _indices;
};

class NodeHashFunction {
public:
    std::size_t operator()(const Node &node) const;
};

std::ostream &operator<<(std::ostream &o, const Node &node);

#endif //MSA_HEURISTIC_SEARCH_NODE_H

