#include "node.h"
#include "input.h"
#include "heuristic.h"
#include <cassert>
#include <unordered_set>
#include <algorithm>

const Sequences test_sequences = {{'A', 'C', 'G', 'H'},
                                  {'C', 'F', 'G'},
                                  {'E', 'A', 'C'}};
const ScoreMatrix test_matrix = {{'-', {{'-', 8}, {'A', 8},  {'C', 8},   {'E', 8},  {'F', 8},  {'G', 8},  {'H', 8}}},
                                 {'A', {{'-', 8}, {'A', -2}, {'C', 2},   {'E', 0},  {'F', 4},  {'G', -1}, {'H', 1}}},
                                 {'C', {{'-', 8}, {'A', 2},  {'C', -12}, {'E', 5},  {'F', 4},  {'G', 3},  {'H', 3}}},
                                 {'E', {{'-', 8}, {'A', 0},  {'C', 5},   {'E', -4}, {'F', 5},  {'G', 0},  {'H', -1}}},
                                 {'F', {{'-', 8}, {'A', 4},  {'C', 4},   {'E', 5},  {'F', -9}, {'G', 5},  {'H', 2}}},
                                 {'G', {{'-', 8}, {'A', -1}, {'C', 3},   {'E', 0},  {'F', 5},  {'G', -5}, {'H', 2}}},
                                 {'H', {{'-', 8}, {'A', 1},  {'C', 3},   {'E', -1}, {'F', 2},  {'G', 2},  {'H', -6}}}};

template<typename T>
void assert_eq(const T &found, const T &expected) {
    if (found != expected) {
        std::cerr << "Expected " << expected << ", found " << found << std::endl;
        assert(false);
    }
}

void test_compute_cost() {
    Node node0({0, 0, 1});
    Node node1({1, 0, 2});
    Node node2({2, 1, 3});
    Node node3({2, 2, 3});
    assert_eq(node0.compute_cost(node1, test_sequences, test_matrix), 14);
    assert_eq(node1.compute_cost(node2, test_sequences, test_matrix), -36);
    assert_eq(node2.compute_cost(node3, test_sequences, test_matrix), 24);
}

void test_get_successors() {
    assert(Node({0, 0, 1}).get_successors(test_sequences) ==
           (std::unordered_set<Node, NodeHashFunction>{Node({0, 0, 2}), Node({0, 1, 1}), Node({0, 1, 2}),
                                                       Node({1, 0, 1}), Node({1, 0, 2}), Node({1, 1, 1}),
                                                       Node({1, 1, 2})}));
    assert(Node({2, 2, 3}).get_successors(test_sequences) ==
           (std::unordered_set<Node, NodeHashFunction>{Node({2, 3, 3}), Node({3, 2, 3}), Node({3, 3, 3})}));
    assert(Node({4, 3, 3}).get_successors(test_sequences).empty());
}

void test_calculate_heuristic() {
    HeuristicCalculator heuristic_calculator(test_sequences, test_matrix);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({4, 3, 3})), 0 + 0 + 0);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({4, 3, 2})), 0 + 8 + 8);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({3, 2, 2})), 2 + 3 + 3);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({1, 2, 2})), 11 + 4 + 3);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({1, 1, 1})), 7 + 10 + 7);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({0, 0, 0})), 3 + 10 + 12);
}

int main() {
    test_compute_cost();
    test_get_successors();
    test_calculate_heuristic();

    return 0;
}

