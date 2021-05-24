#ifndef MSA_HEURISTIC_SEARCH_COMMON_H
#define MSA_HEURISTIC_SEARCH_COMMON_H

#include "input.h"
#include "node.h"
#include <unordered_map>
#include <chrono>

typedef std::vector<Sequence> AlignmentOutput;

std::ostream &operator<<(std::ostream &o, const AlignmentOutput &alignment);

class Open;

class Closed;

class NodeStorage;

class Open {
public:
    Open(Closed *closed, NodeStorage *storage) : _closed(closed), _storage(storage) {};

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

    Closed *_closed;
    NodeStorage *_storage;
    std::priority_queue<std::pair<int, Node>, std::vector<std::pair<int, Node>>, Comparator> _nodes;
    std::unordered_map<Node, int, NodeHashFunction> _g_values;

    void _remove_old_nodes();
};

class Closed {
public:
    Closed(NodeStorage *storage) : _storage(storage) {};

    void add_node(const Node &node, int g);

    void delete_node(const Node &node);

    bool was_expanded(const Node &node);

    int g_value(const Node &node);

    std::vector<Node> get_nodes();

    size_t size() const;

private:
    NodeStorage *_storage;
    std::unordered_map<Node, int, NodeHashFunction> _g_values;
};

class NodeStorage {
public:
    ~NodeStorage();

    void add_node(const Node &node);

    Node *get_node_ptr(const Node &node) const;

    void clear();

    size_t size();

private:
    std::unordered_map<Node, Node *, NodeHashFunction> _nodes;
};

class ProgressTracker {
public:
    explicit ProgressTracker(int max_runtime_secs = 60) :
            _max_nodes_in_memory(0), _iterations_num(0), _max_runtime_secs(max_runtime_secs),
            _start_time(std::chrono::system_clock::now()) {};

    bool on_new_iteration(const Open &open, const Closed &closed);

    int get_max_nodes_in_memory() const;

    int get_iterations_num() const;

    bool need_stop() const;

protected:
    int _max_nodes_in_memory;
    int _iterations_num;
    int _max_runtime_secs;
    std::chrono::system_clock::time_point _start_time;
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
