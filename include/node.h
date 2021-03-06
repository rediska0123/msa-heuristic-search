#ifndef MSA_HEURISTIC_SEARCH_NODE_H
#define MSA_HEURISTIC_SEARCH_NODE_H

#include "input.h"
#include <utility>
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>

class Node;

class NodeHashFunction;

class Node {
public:
    explicit Node(std::vector<int> indices);

    Node(std::vector<int> indices, Node *parent);

    void set_parent(Node *parent) { _parent = parent; }

    std::unordered_set<Node, NodeHashFunction> get_successors(const Sequences &sequences, Node *ptr) const;

    int compute_cost(const Node &other, const Sequences &sequences, const ScoreMatrix &mtx) const;

    friend std::ostream &operator<<(std::ostream &o, const Node &node);

    friend NodeHashFunction;

    bool operator==(const Node &other) const;

    bool operator<(const Node &other) const;

    int operator[](int index) const;

    size_t size() const;

    const Node *get_parent() const;

private:
    std::vector<int> _indices;
    std::size_t _hash;
    Node *_parent;
};

class NodeHashFunction {
public:
    std::size_t operator()(const Node &node) const;
};

std::ostream &operator<<(std::ostream &o, const Node &node);

#endif //MSA_HEURISTIC_SEARCH_NODE_H

