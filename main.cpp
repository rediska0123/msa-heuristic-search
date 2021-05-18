#include <iostream>
#include "node.h"
#include "input.h"
#include "heuristic.h"
#include "common.h"
#include "progressive_alignment.h"
#include "anytime_astar.h"
#include "heuristic_algorithms.h"

using namespace std;

int main() {
    Sequences sequences = {{'A', 'B', 'C'},
                           {'A', 'C', 'D'},
                           {'B', 'A', 'D'}};
    ScoreMatrix matrix = {{'A', {{'A', -1}, {'B', 0},  {'C', 0},  {'D', 0},  {'-', 0}}},
                          {'B', {{'A', 0},  {'B', -1}, {'C', 0},  {'D', 0},  {'-', 0}}},
                          {'C', {{'A', 0},  {'B', 0},  {'C', -1}, {'D', 0},  {'-', 0}}},
                          {'D', {{'A', 0},  {'B', 0},  {'C', 0},  {'D', -1}, {'-', 0}}},
                          {'-', {{'A', 0},  {'B', 0},  {'C', 0},  {'D', 0},  {'-', 0}}}};

    cout << "Progressive alignment:" << endl;
    AlignmentOutput progressive_alignment_output = progressive_alignment(sequences, matrix);
    for (const Sequence &s : progressive_alignment_output) {
        for (const Symbol &c : s)
            cout << c;
        cout << endl;
    }
    cout << calculate_alignment_score(progressive_alignment_output, matrix) << endl << endl;

    cout << "A*:" << endl;
    SearchResult astar_output = AStar(sequences, matrix);
    for (const Sequence &s : astar_output.alignment) {
        for (const Symbol &c : s)
            cout << c;
        cout << endl;
    }
    cout << calculate_alignment_score(astar_output.alignment, matrix) << endl << endl;

    cout << "AnytimeA*:" << endl;
    SearchResult anytime_astar_output = AnytimeAStar(sequences, matrix, 10);
    for (const Sequence &s : anytime_astar_output.alignment) {
        for (const Symbol &c : s)
            cout << c;
        cout << endl;
    }
    cout << calculate_alignment_score(anytime_astar_output.alignment, matrix) << endl;

    return 0;
}

