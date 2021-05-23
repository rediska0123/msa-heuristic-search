#include <iostream>
#include <fstream>
#include <chrono>
#include "node.h"
#include "input.h"
#include "heuristic.h"
#include "common.h"
#include "progressive_alignment.h"
#include "anytime_astar.h"
#include "peastar.h"
#include "astar.h"
#include "idastar.h"

using namespace std;

struct Test {
    Sequences sequences;
    ScoreMatrix mtx;
    AlignmentOutput alignment;
};

struct TestOutput {
    Test test;

    AlignmentOutput progressive_alignment_result;
    double avg_progressive_alignment_runtime_sec;

    SearchResult astar_result;
    double avg_astar_result_runtime_sec;

    SearchResult idastar_result;
    double avg_idastar_runtime_sec;

    std::unordered_map<int, SearchResult> peastar_result; // for different C values
    std::unordered_map<int, double> avg_peastar_runtime_sec;

    std::unordered_map<int, AnytimeAStarSearchResult> anytime_astar_result; // for different W values
    std::unordered_map<int, double> avg_anytime_astar_runtime_sec;
};

std::vector<Test> generate_tests() {
    std::vector<string> all_files;
    std::ifstream f("data/sequences/all_files.txt");
    string filename;
    while (f >> filename)
        all_files.push_back(filename);
    std::vector<Test> tests;
    for (const std::string &sequences_file : all_files)
        for (const std::string &matrix_file : {"data/matrices/BLOSUM45.txt", "data/matrices/PAM250.txt"}) {
            ScoreMatrix mtx = parse_matrix_file(matrix_file);
            std::pair<Sequences, AlignmentOutput> alignment = parse_data_file("data/sequences/" + sequences_file);
            tests.push_back(Test{alignment.first, mtx, alignment.second});
        }
    return tests;
}

int max_sequence_len(const Sequences &seqs) {
    int max_len = 0;
    for (const Sequence &seq : seqs)
        max_len = max(max_len, (int)seq.size());
    return max_len;
}

#define RUN_N_TIMES(run_times, algo_name, run_command) \
auto start_##algo_name = std::chrono::system_clock::now(); \
for (int i = 0; i < (run_times); i++) \
    algo_name##_output = run_command; \
double avg_##algo_name##_run_time = (std::chrono::system_clock::now() - start_##algo_name).count() / (run_times);


std::vector<TestOutput> generate_algorithms_results(const std::vector<Test> &tests, int run_times = 1,
                                                    const std::vector<int> &c_for_peastar = {10}, // TODO
                                                    const std::vector<int> &w_for_anytime_astar = {90}) { // TODO
    std::vector<TestOutput> outputs;
    for (int i = 0; i < (int)tests.size(); i++) {
        const Test &test = tests[i];
        cout << "Test " << i << "/" << (int)tests.size() << ": " << (int)tests[i].alignment.size() <<
                " sequences, max len = " << max_sequence_len(tests[i].sequences) << endl;
        AlignmentOutput progressive_alignment_output;
        cout << " Running progressive alignment..." << endl;
        RUN_N_TIMES(run_times, progressive_alignment, progressive_alignment(test.sequences, test.mtx));

        SearchResult astar_output;
        cout << " Running AStar..." << endl;
        RUN_N_TIMES(run_times, astar, AStar(test.sequences, test.mtx));

        SearchResult idastar_output;
        cout << " Running IDAStar..." << endl;
        RUN_N_TIMES(run_times, idastar, IDAStar(test.sequences, test.mtx));

        std::unordered_map<int, SearchResult> peastar_outputs;
        std::unordered_map<int, double> avg_peastar_run_times;
        for (int c : c_for_peastar) {
            cout << " Running PEAStar(c=" << c << ")..." << endl;
            SearchResult peastar_output;
            RUN_N_TIMES(run_times, peastar, PEAStar(test.sequences, test.mtx, c));
            avg_peastar_run_times[c] = avg_peastar_run_time;
            peastar_outputs[c] = peastar_output;
        }

        std::unordered_map<int, AnytimeAStarSearchResult> anytime_astar_outputs;
        std::unordered_map<int, double> avg_anytime_astar_run_times;
        for (int w : w_for_anytime_astar) {
            cout << " Running AnytimeAStar(w=" << w << ")..." << endl;
            AnytimeAStarSearchResult anytime_astar_output;
            RUN_N_TIMES(run_times, anytime_astar, AnytimeAStar(test.sequences, test.mtx, w));
            avg_anytime_astar_run_times[w] = avg_anytime_astar_run_time;
            anytime_astar_outputs[w] = anytime_astar_output;
        }

        outputs.push_back(TestOutput{test,
                                     progressive_alignment_output, avg_progressive_alignment_run_time,
                                     astar_output, avg_astar_run_time,
                                     idastar_output, avg_idastar_run_time,
                                     peastar_outputs, avg_peastar_run_times,
                                     anytime_astar_outputs, avg_anytime_astar_run_times});
    }
    return outputs;
}

int main() {
    cout << "Generate tests..." << endl;
    std::vector<Test> tests = generate_tests();
    std::vector<TestOutput> outputs = generate_algorithms_results(tests);
    return 0;
}

