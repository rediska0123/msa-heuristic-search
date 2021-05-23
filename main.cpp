#pragma GCC optimize("Ofast,no-stack-protector,unroll-all-loops,unroll-loops")
#pragma GCC target("abm,mmx,sse,sse2,sse3,ssse3,sse4a,popcnt,tune=native")

#include <iostream>
#include <fstream>
#include <chrono>
#include "input.h"
#include "common.h"
#include "progressive_alignment.h"
#include "anytime_astar.h"
#include "peastar.h"
#include "astar.h"
#include "idastar.h"

using namespace std;

struct Test {
    string test_name;
    string matrix_name;
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

    unordered_map<int, SearchResult> peastar_result; // for different C values
    unordered_map<int, double> avg_peastar_runtime_sec;

    unordered_map<int, AnytimeAStarSearchResult> anytime_astar_result; // for different W values
    unordered_map<int, double> avg_anytime_astar_runtime_sec;
};

vector<Test> load_tests() {
    cout << "Loading tests..." << endl;
    vector<string> all_files;
    ifstream f("data/sequences/all_files.txt");
    string filename;
    while (f >> filename)
        all_files.push_back(filename);
    vector<Test> tests;
    for (const string &sequences_file : all_files)
        for (const string &matrix_name : {"BLOSUM45", "PAM250"}) {
            ScoreMatrix mtx = parse_matrix_file("data/matrices/" + matrix_name + ".txt");
            pair<Sequences, AlignmentOutput> alignment = parse_data_file("data/sequences/" + sequences_file);
            tests.push_back(Test{sequences_file, matrix_name, alignment.first, mtx, alignment.second});
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
auto start_##algo_name = chrono::system_clock::now(); \
for (int i = 0; i < (run_times); i++) \
    algo_name##_output = run_command; \
double avg_##algo_name##_run_time = \
    (double)std::chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now() - start_##algo_name).count() /\
            (run_times);


vector<TestOutput> generate_algorithms_results(const vector<Test> &tests, int run_times = 1,
                                                    const vector<int> &c_for_peastar = {10}, // TODO
                                                    const vector<int> &w_for_anytime_astar = {90}) { // TODO
    cout << "Running algorithms..." << endl;
    vector<TestOutput> outputs;
    for (int i = 0; i < (int)tests.size(); i++) {
        const Test &test = tests[i];
        cout << "Test " << i + 1 << "/" << (int)tests.size() << " " << test.test_name << ": " << (int)test.alignment.size() <<
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

        unordered_map<int, SearchResult> peastar_outputs;
        unordered_map<int, double> avg_peastar_run_times;
        for (int c : c_for_peastar) {
            cout << " Running PEAStar(c=" << c << ")..." << endl;
            SearchResult peastar_output;
            RUN_N_TIMES(run_times, peastar, PEAStar(test.sequences, test.mtx, c));
            avg_peastar_run_times[c] = avg_peastar_run_time;
            peastar_outputs[c] = peastar_output;
        }

        unordered_map<int, AnytimeAStarSearchResult> anytime_astar_outputs;
        unordered_map<int, double> avg_anytime_astar_run_times;
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

void save_results(const vector<TestOutput> &outputs) {
    cout << "Saving results..." << endl;
    ofstream f("results.csv");
    f << "Test,Matrix,Sequences number,Maximum sequence len,PA score,PA runtime (sec),"
      << "A* score,A* memory,A* iterations,A* runtime (sec),"
      << "IDA* score,IDA* memory,IDA* iterations,IDA* runtime (sec),";
    for (const pair<const int, SearchResult>& res : outputs[0].peastar_result) {
        const string name = "PEA*(c=" + to_string(res.first) + ")";
        f << name << " score," << name << " memory," << name << " iterations," << name << " runtime (sec),";
    }
    for (const pair<const int, AnytimeAStarSearchResult>& res : outputs[0].anytime_astar_result) {
        const string name = "AnytimeA*(w=" + to_string(res.first) + ")";
        f << name << " score," << name << " memory," << name << " iterations," << name << " runtime (sec),"
          << name << " min bounds," << name << " max bounds,";
    }
    f << "Real alignment score\n";

    for (const TestOutput &outp : outputs) {
        f << outp.test.test_name << "," << outp.test.matrix_name << "," << (int)outp.test.sequences.size() << ","
          << max_sequence_len(outp.test.sequences) << ","
          << calculate_alignment_score(outp.progressive_alignment_result, outp.test.mtx) << ","
          << outp.avg_progressive_alignment_runtime_sec << ",";
        f << calculate_alignment_score(outp.astar_result.alignment, outp.test.mtx) << ","
          << outp.astar_result.max_nodes_in_memory << "," << outp.astar_result.iterations_num << ","
          << outp.avg_astar_result_runtime_sec << ",";
        f << calculate_alignment_score(outp.idastar_result.alignment, outp.test.mtx) << ","
          << outp.idastar_result.max_nodes_in_memory << "," << outp.idastar_result.iterations_num << ","
          << outp.avg_idastar_runtime_sec << ",";
        for (const pair<const int, SearchResult>& res : outp.peastar_result)
            f << calculate_alignment_score(res.second.alignment, outp.test.mtx) << ","
              << res.second.max_nodes_in_memory << "," << res.second.iterations_num << ","
              << outp.avg_peastar_runtime_sec.at(res.first) << ",";
        for (const pair<const int, AnytimeAStarSearchResult>& res : outp.anytime_astar_result) {
            f << calculate_alignment_score(res.second.alignment, outp.test.mtx) << ","
              << res.second.max_nodes_in_memory << "," << res.second.iterations_num << ","
              << outp.avg_anytime_astar_runtime_sec.at(res.first) << ",";
            for (const auto & bound : res.second.bounds)
                f << bound.first << " ";
            f << ",";
            for (const auto & bound : res.second.bounds)
                f << bound.second << " ";
            f << ",";
        }
        f << calculate_alignment_score(outp.test.alignment, outp.test.mtx) << "\n";
    }
}

int main() {
    vector<Test> tests = load_tests();
    vector<TestOutput> outputs = generate_algorithms_results(tests);
    save_results(outputs);

    return 0;
}

