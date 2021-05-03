#ifndef MSA_HEURISTIC_SEARCH_INPUT_H
#define MSA_HEURISTIC_SEARCH_INPUT_H

#include <vector>
#include <unordered_map>

typedef char Symbol;
typedef std::vector<Symbol> Sequence;
typedef std::vector<Sequence> Sequences;

typedef std::unordered_map<int, std::unordered_map<int, int>> ScoreMatrix;

#endif //MSA_HEURISTIC_SEARCH_INPUT_H
