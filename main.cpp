#include <iostream>
#include "node.h"
#include "input.h"

int main() {
    Sequences sequences = {{'A', 'B', 'C'},
                           {'A', 'C', 'E'},
                           {'A', 'B', 'E'}};
    ScoreMatrix matrix = {{'A', {{'A', 1}, {'B', 2}}},
                          {'B', {{'A', 4}, {'B', 6}}}};

    Node node1({1, 2, 3});
    Node node2({1, 2, 5});

    std::cout << node1.compute_cost(node2, matrix) << std::endl;
    for (Node n : node1.get_successors(sequences)) {
        std::cout << n << std::endl;
    }
    std::cout << node1.compute_heuristic(sequences, matrix) << std::endl;

    return 0;
}

