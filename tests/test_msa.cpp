#include "node.h"
#include "input.h"
#include <cassert>
#include <unordered_set>
#include <algorithm>

void test_compute_cost(const Sequences &sequences, const ScoreMatrix &mtx) {
    Node node0({0, 0, 1});
    Node node1({1, 0, 2});
    Node node2({2, 1, 3});
    Node node3({2, 2, 3});
    assert(node0.compute_cost(node1, sequences, mtx) == 14);
    assert(node1.compute_cost(node2, sequences, mtx) == -36);
    assert(node2.compute_cost(node3, sequences, mtx) == 24);
}

void test_get_successors(const Sequences &sequences) {
    Node node0({0, 0, 1});
    Node node1({2, 2, 3});
    std::unordered_set<Node, NodeHashFunction> successors0{Node({0, 0, 2}), Node({0, 1, 1}),
                                                           Node({0, 1, 2}), Node({1, 0, 1}),
                                                           Node({1, 0, 2}), Node({1, 1, 1}),
                                                           Node({1, 1, 2})};
    std::unordered_set<Node, NodeHashFunction> successors1{Node({2, 3, 3}), Node({3, 2, 3}),
                                                           Node({3, 3, 3})};
    assert(successors0 == node0.get_successors(sequences));
    assert(successors1 == node1.get_successors(sequences));
}

int main() {
    Sequences sequences = {{'A', 'C', 'G', 'H'},
                           {'C', 'F', 'G'},
                           {'E', 'A', 'C'}};
    ScoreMatrix matrix = {{'-', {{'-', 8}, {'A', 8},  {'C', 8},   {'E', 8},  {'F', 8},  {'G', 8},  {'H', 8}}},
                          {'A', {{'-', 8}, {'A', -2}, {'C', 2},   {'E', 0},  {'F', 4},  {'G', -1}, {'H', 1}}},
                          {'C', {{'-', 8}, {'A', 2},  {'C', -12}, {'E', 5},  {'F', 4},  {'G', 3},  {'H', 3}}},
                          {'E', {{'-', 8}, {'A', 0},  {'C', 5},   {'E', -4}, {'F', 5},  {'G', 0},  {'H', -1}}},
                          {'F', {{'-', 8}, {'A', 4},  {'C', 4},   {'E', 5},  {'F', -9}, {'G', 5},  {'H', 2}}},
                          {'G', {{'-', 8}, {'A', -1}, {'C', 3},   {'E', 0},  {'F', 5},  {'G', -5}, {'H', 2}}},
                          {'H', {{'-', 8}, {'A', 1},  {'C', 3},   {'E', -1}, {'F', 2},  {'G', 2},  {'H', -6}}},};

    test_compute_cost(sequences, matrix);
    test_get_successors(sequences);
    return 0;
}

