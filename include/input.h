#ifndef MSA_HEURISTIC_SEARCH_INPUT_H
#define MSA_HEURISTIC_SEARCH_INPUT_H

#include <vector>
#include <unordered_map>

#define GAP '-';

typedef char Symbol;
typedef std::vector<Symbol> Sequence;
typedef std::vector<Sequence> Sequences;

typedef std::unordered_map<Symbol, std::unordered_map<Symbol, int>> ScoreMatrix;

#endif //MSA_HEURISTIC_SEARCH_INPUT_H
