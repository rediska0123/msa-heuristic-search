#include <iostream>
#include "node.h"
#include "input.h"
#include "heuristic.h"
#include "common.h"
#include "progressive_alignment.h"
#include "anytime_astar.h"
#include "astar.h"
#include "idastar.h"

using namespace std;

void print_algo_result(const ScoreMatrix &m, const string &algo_name, const AlignmentOutput &outp) {
    cout << algo_name << ": " << endl << outp << calculate_alignment_score(outp, m) << endl << endl;
}

int main() {
    Sequences sequences = {{'A', 'B', 'C'},
                           {'A', 'C', 'D'},
                           {'B', 'A', 'D'}};
    ScoreMatrix matrix = {{'A', {{'A', -1}, {'B', 0},  {'C', 0},  {'D', 0},  {'-', 0}}},
                          {'B', {{'A', 0},  {'B', -1}, {'C', 0},  {'D', 0},  {'-', 0}}},
                          {'C', {{'A', 0},  {'B', 0},  {'C', -1}, {'D', 0},  {'-', 0}}},
                          {'D', {{'A', 0},  {'B', 0},  {'C', 0},  {'D', -1}, {'-', 0}}},
                          {'-', {{'A', 0},  {'B', 0},  {'C', 0},  {'D', 0},  {'-', 0}}}};

    AlignmentOutput progressive_alignment_output = progressive_alignment(sequences, matrix);
    print_algo_result(matrix, "Progressive alignment", progressive_alignment_output);

    SearchResult astar_output = AStar(sequences, matrix);
    print_algo_result(matrix, "A*", astar_output.alignment);

    SearchResult anytime_astar_output = AnytimeAStar(sequences, matrix, 10);
    print_algo_result(matrix, "AnytimeA*", anytime_astar_output.alignment);

    SearchResult idastar_output = IDAStar(sequences, matrix);
    print_algo_result(matrix, "IDA*", idastar_output.alignment);

    return 0;
}

