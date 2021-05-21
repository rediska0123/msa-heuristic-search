#ifndef MSA_HEURISTIC_SEARCH_COMMON_H
#define MSA_HEURISTIC_SEARCH_COMMON_H

#include "input.h"
#include "node.h"
#include <memory>

typedef std::vector<Sequence> AlignmentOutput;

std::ostream &operator<<(std::ostream &o, const AlignmentOutput &alignment);

class Open;

class Closed;

class Open {
public:
    explicit Open(std::shared_ptr<Closed> closed) : _closed(std::move(closed)) {};

    std::tuple<Node, int, int> get_best_node();

    void add_node(const Node &node, int g, int f);

    bool is_empty();

    std::vector<Node> get_nodes();

    size_t size() const;

private:
    struct Comparator {
        constexpr bool operator()(
                std::pair<int, Node> const &a,
                std::pair<int, Node> const &b)
        const noexcept {
            return a.first > b.first;
        }
    };

    std::shared_ptr<Closed> _closed;
    std::priority_queue<std::pair<int, Node>, std::vector<std::pair<int, Node>>, Comparator> _nodes;
    std::unordered_map<Node, int, NodeHashFunction> _g_values;

    void _remove_old_nodes();
};

class Closed {
public:
    void add_node(const Node &node, int g);

    void delete_node(const Node &node);

    bool was_expanded(const Node &node);

    int g_value(const Node &node);

    std::vector<Node> get_nodes();

    size_t size() const;

private:
    std::unordered_map<Node, int, NodeHashFunction> _g_values;
};

class ProgressTracker {
public:
    ProgressTracker() = default;

    void on_new_iteration(const Open &open, const Closed &closed);

    int get_max_nodes_in_memory() const;

    int get_iterations_num() const;
protected:
    int _max_nodes_in_memory;
    int _iterations_num;
};

struct SearchResult {
    SearchResult() = default;
    SearchResult(const AlignmentOutput &a, const ProgressTracker &tracker);
    AlignmentOutput alignment;
    int max_nodes_in_memory;
    int iterations_num;
};

int calculate_alignment_score(const AlignmentOutput &alignment, const ScoreMatrix &mtx);

std::pair<Node, Node> get_start_and_goal_nodes(const Sequences &sequences);

std::vector<Node> get_path(Node *node);

AlignmentOutput path_to_alignment(const Sequences &sequences, const std::vector<Node> &path);

std::pair<Sequences, AlignmentOutput> parse_data_file(const std::string &filepath);

ScoreMatrix parse_matrix_file(const std::string &filepath);

#endif //MSA_HEURISTIC_SEARCH_COMMON_H
