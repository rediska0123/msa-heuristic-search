#include "node.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>

std::unordered_set<Node, NodeHashFunction> Node::get_successors(const Sequences &sequences) const {
    std::shared_ptr<Node> parent = std::make_shared<Node>(*this);
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
        successors.insert(Node(std::move(indices), std::shared_ptr<Node>(parent)));
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
    std::size_t seed = node._indices.size();
    for (int idx: node._indices) {
        seed ^= idx + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
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
    return _parent.get();
}
