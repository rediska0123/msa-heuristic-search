#include "common.h"
#include "node.h"
#include <algorithm>
#include <fstream>
#include <cassert>

std::ostream &operator<<(std::ostream &o, const AlignmentOutput &alignment) {
    for (const Sequence &s : alignment) {
        for (const Symbol &c : s)
            o << c;
        o << "\n";
    }
    return o;
}

std::tuple<Node, int, double> Open::get_best_node() {
    auto[f, best_node] = *_sorted_nodes.begin();
    int g = _f_g_values[best_node].second;
    _sorted_nodes.erase(_sorted_nodes.begin());
    _f_g_values.erase(best_node);
    return {best_node, g, f};
}

void Open::add_node(const Node &node, int g, double f) {
    auto f_g_node = _f_g_values.find(node);
    if (f_g_node == _f_g_values.end() || f_g_node->second.second > g) {
        if (f_g_node != _f_g_values.end())
            _sorted_nodes.erase({f_g_node->second.first, node});
        _sorted_nodes.insert({f, node});
        _f_g_values[node] = {f, g};
        _storage->add_node(node);
    }
}

bool Open::is_empty() {
    return _sorted_nodes.empty();
}

size_t Open::size() const {
    return _sorted_nodes.size();
}

void Closed::delete_node(const Node &node) {
    _g_values.erase(node);
}

bool Closed::was_expanded(const Node &node) {
    return _g_values.count(node) != 0;
}

int Closed::g_value(const Node &node) {
    return _g_values[node];
}

void Closed::add_node(const Node &node, int g) {
    _g_values[node] = g;
    _storage->add_node(node);
}

size_t Closed::size() const {
    return _g_values.size();
}

void NodeStorage::add_node(const Node &node) {
    if (_nodes.find(node) != _nodes.end()) {
        *_nodes.at(node) = node;
    } else {
        _nodes.insert({node, new Node(node)});
    }
}

Node* NodeStorage::get_node_ptr(const Node &node) const {
    return _nodes.at(node);
}

NodeStorage::~NodeStorage() {
    clear();
}

void NodeStorage::clear() {
    for(auto [_, ptr]: _nodes) {
        delete ptr;
    }
    _nodes.clear();
}

size_t NodeStorage::size() {
    return _nodes.size();
}

int calculate_alignment_score(const AlignmentOutput &alignment, const ScoreMatrix &mtx) {
    int score = 0;
    if (alignment.empty())
        return 0;
    for (int pos = 0; pos < (int) alignment[0].size(); pos++)
        for (int i = 0; i < (int) alignment.size(); i++)
            for (int j = 0; j < i; j++)
                score += mtx.at(alignment[i][pos]).at(alignment[j][pos]);
    return score;
}

std::pair<Node, Node> get_start_and_goal_nodes(const Sequences &sequences) {
    Node start_node = Node(std::vector<int>((int) sequences.size(), 0));
    std::vector<int> goal_indices = std::vector<int>((int) sequences.size(), 0);
    std::transform(sequences.begin(), sequences.end(), goal_indices.begin(),
                   [](const Sequence &s) { return s.size(); });
    Node goal_node = Node(std::move(goal_indices));
    return {start_node, goal_node};
}

std::vector<Node> get_path(Node *node) {
    std::vector<Node> path;
    const Node *parent = node;
    while (parent != nullptr) {
        path.push_back(*parent);
        parent = parent->get_parent();
    }
    std::reverse(path.begin(), path.end());
    return path;
}

AlignmentOutput path_to_alignment(const Sequences &sequences, const std::vector<Node> &path) {
    AlignmentOutput output = std::vector<std::vector<Symbol>>(sequences.size(), std::vector<Symbol>(path.size() - 1));
    for (int i = 1; i < path.size(); ++i) {
        for (int j = 0; j < sequences.size(); ++j) {
            if (path[i][j] - path[i - 1][j] == 0) {
                output[j][i - 1] = GAP;
            } else {
                output[j][i - 1] = sequences[j][path[i - 1][j]];
            }
        }
    }
    return output;
}

std::pair<Sequences, AlignmentOutput> parse_data_file(const std::string &filepath) {
    std::ifstream f(filepath);
    std::vector<Sequence> all_seqs;
    for (std::string line; getline(f, line);) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);
        std::vector<Symbol> seq;
        for (Symbol c : line)
            seq.push_back(c);
        all_seqs.push_back(seq);
    }
    assert(!all_seqs.empty()), "Empty file";
    Sequences seqs;
    AlignmentOutput outp;
    for (int i = 0; i < (int) all_seqs.size() / 2; i++)
        seqs.push_back(all_seqs[i]);
    for (int i = (int) all_seqs.size() / 2; i < (int) all_seqs.size(); i++)
        outp.push_back(all_seqs[i]);
    return {seqs, outp};
}

ScoreMatrix parse_matrix_file(const std::string &filepath) {
    std::ifstream f(filepath);
    int symbols_num;
    f >> symbols_num;
    std::vector<char> symbols(symbols_num + 1);
    for (int i = 0; i < symbols_num + 1; i++)
        f >> symbols[i];
    ScoreMatrix m;
    for (char i : symbols)
        for (char j : symbols) {
            f >> m[i][j];
            m[i][j] = -m[i][j];
        }
    return m;
}

bool ProgressTracker::on_new_iteration(const Open &open, const Closed &closed) {
    _max_nodes_in_memory = std::max(_max_nodes_in_memory, (int)open.size() + (int)closed.size());
    _iterations_num += 1;
    return need_stop();
}

bool ProgressTracker::need_stop() const {
    bool stop = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _start_time).count() > _max_runtime_secs;
    if (stop)
        std::cerr << "Interrupted, timeout reached (" << _max_runtime_secs << "s)" << std::endl;
    return stop;
}

int ProgressTracker::get_max_nodes_in_memory() const {
    return _max_nodes_in_memory;
}

int ProgressTracker::get_iterations_num() const {
    return _iterations_num;
}

SearchResult::SearchResult(const AlignmentOutput &a, const ProgressTracker &tracker) {
    alignment = a;
    max_nodes_in_memory = tracker.get_max_nodes_in_memory();
    iterations_num = tracker.get_iterations_num();
}
