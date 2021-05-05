#include <iostream>
#include "node.h"
#include "input.h"
#include "heuristic.h"

using namespace std;

int main() {
    Sequences sequences = {{'A', 'B', 'C'},
                           {'A', 'C', 'D'},
                           {'B', 'A', 'D'}};
    ScoreMatrix matrix = {{'A', {{'A', -1}, {'B', 0},  {'C', 0},  {'D', 0},  {'-', 0}}},
                          {'B', {{'A', 0},  {'B', -1}, {'C', 0},  {'D', 0},  {'-', 0}}},
                          {'C', {{'A', 0},  {'B', 0},  {'C', -1}, {'D', 0},  {'-', 0}}},
                          {'D', {{'A', 0},  {'B', 0},  {'C', 0},  {'D', -1}, {'-', 0}}},
                          {'-', {{'A', 0},  {'B', 0},  {'C', 0},  {'D', 0},  {'-', -1}}}};

    Node node1({1, 1, 0});
    Node node2({2, 1, 1});

    cout << node1.compute_cost(node2, sequences, matrix) << endl;
    for (const Node &n : node1.get_successors(sequences)) {
        cout << n << endl;
    }

    HeuristicCalculator heuristic_calculator(sequences, matrix);
    cout << heuristic_calculator.calculate_heuristic(node1) << endl;

    return 0;
}

