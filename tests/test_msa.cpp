#include "node.h"
#include "heuristic.h"
#include "common.h"
#include "progressive_alignment.h"
#include "astar.h"
#include "peastar.h"
#include "anytime_astar.h"
#include "idastar.h"
#include <cassert>
#include <unordered_set>
#include <algorithm>

using namespace std;

const Sequences test_sequences1 = {{'A', 'C', 'G', 'H'},
                                   {'C', 'F', 'G'},
                                   {'E', 'A', 'C'}};
const AlignmentOutput test_aligned_sequences1 = {{GAP, 'A', 'C', GAP, 'G', 'H'},
                                                 {GAP, GAP, 'C', 'F', 'G', GAP},
                                                 {'E', 'A', 'C', GAP, GAP, GAP}};
const ScoreMatrix test_matrix1 = {{'-', {{'-', 8}, {'A', 8},  {'C', 8},   {'E', 8},  {'F', 8},  {'G', 8},  {'H', 8}}},
                                  {'A', {{'-', 8}, {'A', -2}, {'C', 2},   {'E', 0},  {'F', 4},  {'G', -1}, {'H', 1}}},
                                  {'C', {{'-', 8}, {'A', 2},  {'C', -12}, {'E', 5},  {'F', 4},  {'G', 3},  {'H', 3}}},
                                  {'E', {{'-', 8}, {'A', 0},  {'C', 5},   {'E', -4}, {'F', 5},  {'G', 0},  {'H', -1}}},
                                  {'F', {{'-', 8}, {'A', 4},  {'C', 4},   {'E', 5},  {'F', -9}, {'G', 5},  {'H', 2}}},
                                  {'G', {{'-', 8}, {'A', -1}, {'C', 3},   {'E', 0},  {'F', 5},  {'G', -5}, {'H', 2}}},
                                  {'H', {{'-', 8}, {'A', 1},  {'C', 3},   {'E', -1}, {'F', 2},  {'G', 2},  {'H', -6}}}};

const Sequences test_sequences2 = {{'A', 'A'},
                                   {'A', 'A'},
                                   {'A'}};
const ScoreMatrix test_matrix2 = {{'A', {{'A', 1}, {'-', 1}}},
                                  {'-', {{'A', 1}, {'-', 0}}}};

const Sequences test_sequences3 = {{'A'},
                                   {'A', 'A'},
                                   {'A', 'A', 'A'}};
const ScoreMatrix test_matrix3 = {{'-', {{'-', 1}, {'A', 2}}},
                                  {'A', {{'-', 2}, {'A', -1}}}};

template<typename T>
void assert_eq(const T &found, const T &expected) {
    if (found != expected) {
        cerr << "Found " << found << ", expected " << expected << endl;
        assert(false);
    }
}

void test_compute_cost() {
    Node node0({0, 0, 1});
    Node node1({1, 0, 2});
    Node node2({2, 1, 3});
    Node node3({2, 2, 3});
    assert_eq(node0.compute_cost(node1, test_sequences1, test_matrix1), 14);
    assert_eq(node1.compute_cost(node2, test_sequences1, test_matrix1), -36);
    assert_eq(node2.compute_cost(node3, test_sequences1, test_matrix1), 24);
}

void test_get_successors() {
    assert(Node({0, 0, 1}).get_successors(test_sequences1, nullptr) ==
           (unordered_set<Node, NodeHashFunction>{Node({0, 0, 2}), Node({0, 1, 1}), Node({0, 1, 2}),
                                                  Node({1, 0, 1}), Node({1, 0, 2}), Node({1, 1, 1}),
                                                  Node({1, 1, 2})}));
    assert(Node({2, 2, 3}).get_successors(test_sequences1, nullptr) ==
           (unordered_set<Node, NodeHashFunction>{Node({2, 3, 3}), Node({3, 2, 3}), Node({3, 3, 3})}));
    assert(Node({4, 3, 3}).get_successors(test_sequences1, nullptr).empty());
}

void test_calculate_heuristic() {
    HeuristicCalculator heuristic_calculator(test_sequences1, test_matrix1);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({4, 3, 3})), 0 + 0 + 0);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({4, 3, 2})), 0 + 8 + 8);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({3, 2, 2})), 2 + 3 + 3);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({1, 2, 2})), 11 + 4 + 3);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({1, 1, 1})), 7 + 10 + 7);
    assert_eq(heuristic_calculator.calculate_heuristic(Node({0, 0, 0})), 3 + 10 + 12);
}

void test_calculate_alignment_score() {
    assert_eq(calculate_alignment_score(test_aligned_sequences1, test_matrix1), 61);
}

void test_progressive_alignment() {
    assert(progressive_alignment(test_sequences1, test_matrix1) ==
           AlignmentOutput({Sequence({'-', 'A', 'C', 'G', 'H'}),
                            Sequence({'-', '-', 'C', 'F', 'G'}),
                            Sequence({'E', 'A', 'C', '-', '-'})}));
    assert_eq(calculate_alignment_score(progressive_alignment(test_sequences1, test_matrix1), test_matrix1), 41);
}

void test_AStar() {
    SearchResult result = AStar(test_sequences1, test_matrix1);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix1), 36);

    result = AStar(test_sequences2, test_matrix2);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix2), 6);

    result = AStar(test_sequences3, test_matrix3);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix3), 5);
}

void test_PEAStar() {
    SearchResult result = PEAStar(test_sequences1, test_matrix1, 0);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix1), 36);

    result = PEAStar(test_sequences2, test_matrix2, 20);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix2), 6);

    result = PEAStar(test_sequences3, test_matrix3, 10);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix3), 5);
}

void test_bounds(const AnytimeAStarSearchResult &res, const ScoreMatrix &m) {
    int score = calculate_alignment_score(res.alignment, m);
    for (const std::pair <int, int> &a : res.bounds) {
        assert(a.first <= score);
        assert(a.second >= score);
    }
}

void test_AnytimeAStar() {
    AnytimeAStarSearchResult result = AnytimeAStar(test_sequences1, test_matrix1, 10);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix1), 36);
    test_bounds(result, test_matrix1);

    result = AnytimeAStar(test_sequences2, test_matrix2, 9);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix2), 6);
    test_bounds(result, test_matrix2);

    result = AnytimeAStar(test_sequences3, test_matrix3, 3);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix3), 5);
    test_bounds(result, test_matrix3);
}

void test_IDAStar() {
    SearchResult result = IDAStar(test_sequences1, test_matrix1);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix1), 36);

    result = IDAStar(test_sequences2, test_matrix2);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix2), 6);

    result = IDAStar(test_sequences3, test_matrix3);
    assert_eq(calculate_alignment_score(result.alignment, test_matrix3), 5);
}

std::pair<Sequences, ScoreMatrix> gen_simple_test() {
    std::vector<Symbol> alphabet = {'A', 'B', 'C', '-'};
    int n = rand() % 4 + 1;
    Sequences seqs = {};
    for (int i = 0; i < n; i++) {
        seqs.push_back(Sequence(rand() % 5 + 1));
        for (int j = 0; j < (int) seqs.back().size(); j++)
            seqs[i][j] = alphabet[rand() % 3];
    }
    ScoreMatrix m;
    for (int i = 0; i < (int) alphabet.size(); i++)
        for (int j = i; j < (int) alphabet.size(); j++)
            m[alphabet[i]][alphabet[j]] = m[alphabet[j]][alphabet[i]] = rand() % 21 - 1;
    m['-']['-'] = rand() % 11;  // must be non-negative for monotone heuristic
    return {seqs, m};
}

void test_same_aligment_scores() {
    for (int iter = 0; iter < 50; iter++) {
        auto[seqs, m] = gen_simple_test();
        SearchResult astar = AStar(seqs, m);
        SearchResult peastar = PEAStar(seqs, m, 10);
        AnytimeAStarSearchResult anytime_astar = AnytimeAStar(seqs, m, double(rand() % 10 + 1));
        SearchResult idastar = IDAStar(seqs, m);
        int astar_score = calculate_alignment_score(astar.alignment, m);
        int peastar_score = calculate_alignment_score(peastar.alignment, m);
        int anytime_astar_score = calculate_alignment_score(anytime_astar.alignment, m);
        int idastar_score = calculate_alignment_score(idastar.alignment, m);
        assert_eq(astar_score, peastar_score);
        assert_eq(astar_score, anytime_astar_score);
        assert_eq(astar_score, idastar_score);
        test_bounds(anytime_astar, m);
    }
}

int main() {
    test_compute_cost();
    test_get_successors();
    test_calculate_heuristic();
    test_calculate_alignment_score();
    test_progressive_alignment();
    test_AStar();
    test_PEAStar();
    test_AnytimeAStar();
    test_IDAStar();
    test_same_aligment_scores();

    return 0;
}

