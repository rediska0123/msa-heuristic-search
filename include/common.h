#ifndef MSA_HEURISTIC_SEARCH_COMMON_H
#define MSA_HEURISTIC_SEARCH_COMMON_H

#include "input.h"
#include "node.h"
#include <memory>

typedef std::vector<Sequence> AlignmentOutput;

std::ostream &operator<<(std::ostream &o, const AlignmentOutput &alignment);

class Open {
public:
    virtual std::tuple<Node, int, int> get_best_node() = 0;

    virtual std::vector<Node> get_nodes() = 0;

    virtual bool is_empty() = 0;
};

class Closed {
public:
    virtual bool was_expanded(const Node &node) = 0;

    virtual std::vector<Node> get_nodes() = 0;
};

struct SearchResult {
    AlignmentOutput alignment;
    std::shared_ptr<Open> open;
    std::shared_ptr<Closed> closed;
};

int calculate_alignment_score(const AlignmentOutput &alignment, const ScoreMatrix &mtx);

std::pair<Node, Node> get_start_and_goal_nodes(const Sequences &sequences);

std::vector<Node> get_path(Node *node);

AlignmentOutput path_to_alignment(const Sequences &sequences, const std::vector<Node> &path);

#endif //MSA_HEURISTIC_SEARCH_COMMON_H
