#include <iostream>
#include "node.h"
#include "input.h"

int main() {
    Sequences sequences = {{'A', 'B', 'C'},
                           {'A', 'C', 'E'},
                           {'B', 'A', 'E'}};
    ScoreMatrix matrix = {{'A', {{'A', 1}, {'B', 2}, {'-', 3}}},
                          {'B', {{'A', 4}, {'B', 6}, {'-', 3}}},
                          {'-', {{'A', 3}, {'B', 3}, {'-', 3}}}};

    Node node1({1, 1, 0});
    Node node2({2, 1, 1});

    std::cout << node1.compute_cost(node2, sequences, matrix) << std::endl;
    for (Node n : node1.get_successors(sequences)) {
        std::cout << n << std::endl;
    }
    std::cout << node1.compute_heuristic(sequences, matrix) << std::endl;

    return 0;
}

