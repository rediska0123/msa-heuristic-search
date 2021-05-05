#ifndef MSA_HEURISTIC_SEARCH_INPUT_H
#define MSA_HEURISTIC_SEARCH_INPUT_H

#include <vector>
#include <unordered_map>

typedef char Symbol;
typedef std::vector<Symbol> Sequence;
typedef std::vector<Sequence> Sequences;

const Symbol GAP = '-';

typedef std::unordered_map<Symbol, std::unordered_map<Symbol, int>> ScoreMatrix;

#endif //MSA_HEURISTIC_SEARCH_INPUT_H
