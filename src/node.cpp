#include "node.h"
#include "utils.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>

Node::Node(std::vector<int> indices) : _indices(std::move(indices)), _parent(nullptr) {
    _hash = calculate_int_vector_hash(_indices);
}

Node::Node(std::vector<int> indices, Node *parent) : _indices(std::move(indices)), _parent(parent) {
    _hash = calculate_int_vector_hash(_indices);
}

std::unordered_set<Node, NodeHashFunction> Node::get_successors(const Sequences &sequences, Node *ptr) const {
    unsigned int mask = ~0;
    for (int i = 0; i < _indices.size(); ++i) {
        if (_indices[i] == sequences[i].size()) {
            mask ^= 1 << i;
        }
    }
    std::unordered_set<unsigned int> edges;
    for (unsigned int direction = 1; direction < 1 << sequences.size(); ++direction) {
        unsigned int edge = direction & mask;
        if (edge != 0) {
            edges.insert(edge);
        }
    }
    std::unordered_set<Node, NodeHashFunction> successors;
    for (unsigned int edge: edges) {
        std::vector<int> indices;
        std::copy(_indices.begin(), _indices.end(), std::back_inserter(indices));
        for (int i = 0; i < _indices.size(); ++i) {
            indices[i] += (edge & (1 << i)) >> i;
        }
        successors.insert(Node(std::move(indices), ptr));
    }
    return successors;
}

int Node::compute_cost(const Node &other, const Sequences &sequences, const ScoreMatrix &mtx) const {
    std::vector<Symbol> symbols(_indices.size());
    for (int i = 0; i < _indices.size(); ++i) {
        if (other._indices[i] - _indices[i] == 0) {
            symbols[i] = GAP;
        } else if (other._indices[i] - _indices[i] == 1) {
            symbols[i] = sequences[i][_indices[i]];
        } else {
            throw std::runtime_error("Can't compute cost between vertices which aren't endpoints of directed edge");
        }
    }
    int cost = 0;
    for (int i = 0; i < symbols.size(); ++i) {
        for (int j = i + 1; j < symbols.size(); ++j) {
            cost += mtx.at(symbols[i]).at(symbols[j]);
        }
    }
    return cost;
}

bool Node::operator==(const Node &other) const {
    return _indices == other._indices;
}

std::size_t NodeHashFunction::operator()(const Node &node) const {
    return node._hash;
}

std::ostream &operator<<(std::ostream &o, const Node &node) {
    for (int i : node._indices)
        o << i << " ";
    return o;
}

int Node::operator[](int index) const {
    return _indices.at(index);
}

size_t Node::size() const {
    return _indices.size();
}

const Node *Node::get_parent() const {
    return _parent;
}

bool Node::operator<(const Node &other) const {
    if ((int)_indices.size() != (int)other._indices.size())
        return (int)_indices.size() < (int)other._indices.size();
    for (int i = 0; i < (int)_indices.size(); i++)
        if (_indices[i] != other._indices[i])
            return _indices[i] < other._indices[i];
    return false;
}
