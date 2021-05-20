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

std::tuple<Node, int, int> Open::get_best_node() {
    _remove_old_nodes();
    auto[f, best_node] = _nodes.top();
    _nodes.pop();
    return {best_node, _g_values[best_node], f};
}

void Open::add_node(const Node &node, int g, int f) {
    if (_g_values.find(node) == _g_values.end() || _g_values[node] >= g) {
        _nodes.push({f, node});
        _g_values[node] = g;
    }
}

bool Open::is_empty() {
    _remove_old_nodes();
    return _nodes.empty();
}

void Open::_remove_old_nodes() {
    while (!_nodes.empty() && _closed->was_expanded(_nodes.top().second)) {
        _nodes.pop();
    }
}

std::vector<Node> Open::get_nodes() {
    std::vector<Node> nodes;
    std::priority_queue<std::pair<int, Node>, std::vector<std::pair<int, Node>>, Comparator> tmp = _nodes;
    while (!tmp.empty()) {
        nodes.push_back(tmp.top().second);
        tmp.pop();
    }
    return nodes;
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
}

std::vector<Node> Closed::get_nodes() {
    std::vector<Node> nodes;
    for (std::pair<Node, int> p : _g_values)
        nodes.push_back(p.first);
    return nodes;
}

int calculate_alignment_score(const AlignmentOutput &alignment, const ScoreMatrix &mtx) {
    int score = 0;
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
        std::vector<char> seq;
        for (char c : line)
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