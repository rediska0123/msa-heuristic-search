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
    double avg_progressive_alignment_runtime_ms;

    SearchResult astar_result;
    double avg_astar_result_runtime_ms;

    SearchResult idastar_result;
    double avg_idastar_runtime_ms;

    vector<SearchResult> peastar_result; // for different C values
    vector<double> avg_peastar_runtime_ms;

    vector<AnytimeAStarSearchResult> anytime_astar_result; // for different W values
    vector<double> avg_anytime_astar_runtime_ms;
};

vector<Test> load_tests(string path, const vector<string> &matrices) {
    cout << "Loading tests..." << endl;
    vector<string> all_files;
    ifstream f(path);
    string filename;
    while (f >> filename)
        all_files.push_back(filename);
    vector<Test> tests;
    for (const string &sequences_file : all_files)
        for (const string &matrix_name : matrices) {
            ScoreMatrix mtx = parse_matrix_file("data/matrices/" + matrix_name + ".txt");
            pair<Sequences, AlignmentOutput> alignment = parse_data_file("data/sequences/" + sequences_file);
            tests.push_back(Test{sequences_file, matrix_name, alignment.first, mtx, alignment.second});
        }
    return tests;
}

int max_sequence_len(const Sequences &seqs) {
    int max_len = 0;
    for (const Sequence &seq : seqs)
        max_len = max(max_len, (int) seq.size());
    return max_len;
}

void save_table_header(string path, const vector<int> &c_for_peastar, const vector<double> &w_for_anytime_astar) {
    ofstream f(path);
    f << "Test,Sequence lengths,Matrix,Sequences number,Maximum sequence len,PA score,PA runtime (micros),"
      << "A* score,A* memory,A* iterations,A* runtime (micros),"
      << "IDA* score,IDA* memory,IDA* iterations,IDA* runtime (micros),";
    for (int c : c_for_peastar) {
        const string name = "PEA*(c=" + to_string(c) + ")";
        f << name << " score," << name << " memory," << name << " iterations," << name << " runtime (micros),";
    }
    for (double w : w_for_anytime_astar) {
        const string name = "AnytimeA*(w=" + to_string(w) + ")";
        f << name << " score," << name << " memory," << name << " iterations," << name << " runtime (micros),"
          << name << " min bounds," << name << " max bounds,";
    }
    f << "Real alignment score\n";
    f.close();
}

void save_new_output(string path, const TestOutput &outp,
                     const vector<int> &c_for_peastar,
                     const vector<double> &w_for_anytime_astar) {
    ofstream f(path, ios::out | ios::app);
    f << outp.test.test_name << ",";
    for (const Sequence &s: outp.test.sequences)
        f << (int) s.size() << " ";
    f << "," << outp.test.matrix_name << "," << (int) outp.test.sequences.size() << ","
      << max_sequence_len(outp.test.sequences) << ","
      << calculate_alignment_score(outp.progressive_alignment_result, outp.test.mtx) << ","
      << outp.avg_progressive_alignment_runtime_ms << ",";
    f << calculate_alignment_score(outp.astar_result.alignment, outp.test.mtx) << ","
      << outp.astar_result.max_nodes_in_memory << "," << outp.astar_result.iterations_num << ","
      << outp.avg_astar_result_runtime_ms << ",";
    f << calculate_alignment_score(outp.idastar_result.alignment, outp.test.mtx) << ","
      << outp.idastar_result.max_nodes_in_memory << "," << outp.idastar_result.iterations_num << ","
      << outp.avg_idastar_runtime_ms << ",";
    for (int i = 0; i < (int) c_for_peastar.size(); i++)
        f << calculate_alignment_score(outp.peastar_result[i].alignment, outp.test.mtx) << ","
          << outp.peastar_result[i].max_nodes_in_memory << "," << outp.peastar_result[i].iterations_num << ","
          << outp.avg_peastar_runtime_ms[i] << ",";
    for (int i = 0; i < (int) w_for_anytime_astar.size(); i++) {
        f << calculate_alignment_score(outp.anytime_astar_result[i].alignment, outp.test.mtx) << ","
          << outp.anytime_astar_result[i].max_nodes_in_memory << "," << outp.anytime_astar_result[i].iterations_num
          << ","
          << outp.avg_anytime_astar_runtime_ms[i] << ",";
        for (const auto &bound : outp.anytime_astar_result[i].bounds)
            f << bound.first << " ";
        f << ",";
        for (const auto &bound : outp.anytime_astar_result[i].bounds)
            f << bound.second << " ";
        f << ",";
    }
    f << calculate_alignment_score(outp.test.alignment, outp.test.mtx) << "\n";
    f.close();
}

int get_test_timeout(const Test &test) {
    return 5;
}

#define RUN_N_TIMES(run_times, algo_name, run_command) \
auto start_##algo_name = chrono::system_clock::now(); \
for (int i = 0; i < (run_times); i++) \
    algo_name##_output = run_command; \
double avg_##algo_name##_run_time = \
    (double)std::chrono::duration_cast<std::chrono::microseconds>(chrono::system_clock::now() - start_##algo_name).count() /\
            ((double)run_times);


void generate_algorithms_results(string path,
                                 const vector<Test> &tests,
                                 const vector<int> &c_for_peastar,
                                 const vector<double> &w_for_anytime_astar,
                                 int run_times = 1) {
    cout << "Running algorithms..." << endl;

    save_table_header(path, c_for_peastar, w_for_anytime_astar);

    for (int i = 0; i < (int)tests.size(); i++) {
        const Test &test = tests[i];
        int timeout = get_test_timeout(test);
        cout << "Test " << i + 1 << "/" << (int)tests.size() << " " << test.test_name << ": " << (int)test.alignment.size() <<
                " sequences, max len = " << max_sequence_len(tests[i].sequences) << ", timeout(s) = " << timeout << endl;
        AlignmentOutput progressive_alignment_output;
        cout << " Running progressive alignment..." << endl;
        RUN_N_TIMES(run_times, progressive_alignment, progressive_alignment(test.sequences, test.mtx));

        SearchResult astar_output;
        cout << " Running AStar..." << endl;
        RUN_N_TIMES(run_times, astar, AStar(test.sequences, test.mtx, timeout));

        SearchResult idastar_output;
        cout << " Running IDAStar..." << endl;
        RUN_N_TIMES(run_times, idastar, IDAStar(test.sequences, test.mtx, timeout));

        vector<SearchResult> peastar_outputs;
        vector<double> avg_peastar_run_times;
        for (int c : c_for_peastar) {
            cout << " Running PEAStar(c=" << c << ")..." << endl;
            SearchResult peastar_output;
            RUN_N_TIMES(run_times, peastar, PEAStar(test.sequences, test.mtx, c, timeout));
            avg_peastar_run_times.push_back(avg_peastar_run_time);
            peastar_outputs.push_back(peastar_output);
        }

        vector<AnytimeAStarSearchResult> anytime_astar_outputs;
        vector<double> avg_anytime_astar_run_times;
        for (double w : w_for_anytime_astar) {
            cout << " Running AnytimeAStar(w=" << w << ")..." << endl;
            AnytimeAStarSearchResult anytime_astar_output;
            RUN_N_TIMES(run_times, anytime_astar,
                        AnytimeAStar(test.sequences, test.mtx, w, timeout));
            avg_anytime_astar_run_times.push_back(avg_anytime_astar_run_time);
            anytime_astar_outputs.push_back(anytime_astar_output);
        }

        save_new_output(path, TestOutput{test,
                                   progressive_alignment_output, avg_progressive_alignment_run_time,
                                   astar_output, avg_astar_run_time,
                                   idastar_output, avg_idastar_run_time,
                                   peastar_outputs, avg_peastar_run_times,
                                   anytime_astar_outputs, avg_anytime_astar_run_times},
                        c_for_peastar,
                        w_for_anytime_astar);
    }
}

int main() {
    vector<Test> tests;
    vector<int> c_for_peastar = {30, 50, 70};
    vector<double> w_for_anytime_astar = {100.0 / 99.0, 1.1, 1.5, 2};
    tests = load_tests("data/sequences/all_files.txt", {"PAM250"});
    generate_algorithms_results("results.csv", tests, c_for_peastar, w_for_anytime_astar);

    c_for_peastar.clear();
    for (int i = 0; i < 100; i += 5) {
        c_for_peastar.push_back(i);
    }
    tests = load_tests("data/test_config.txt", {"PAM250"});
    generate_algorithms_results("single_test_results.csv", tests, c_for_peastar, w_for_anytime_astar);
    return 0;
}

