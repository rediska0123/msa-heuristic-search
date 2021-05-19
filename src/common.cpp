#include "common.h"
#include "node.h"
#include <algorithm>

std::ostream &operator<<(std::ostream &o, const AlignmentOutput &alignment) {
    for (const Sequence &s : alignment) {
        for (const Symbol &c : s)
            o << c;
        o << "\n";
    }
    return o;
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
    Node start_node = Node(std::vector<int>((int)sequences.size(), 0));
    std::vector<int> goal_indices = std::vector<int>((int)sequences.size(), 0);
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

