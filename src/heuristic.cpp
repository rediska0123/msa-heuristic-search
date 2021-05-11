#include "heuristic.h"
#include "utils.h"

void relax(int &val, int new_val) {
    if (val > new_val)
        val = new_val;
}

HeuristicCalculator::HeuristicCalculator(const Sequences &sequences, const ScoreMatrix &mtx) {
    int n = (int) sequences.size();
    _pair_heuristic.resize(n);
    for (int i = 0; i < n; i++) {
        _pair_heuristic[i].resize(i);
        for (int j = 0; j < i; j++) {
            const Sequence &s1 = sequences[i], &s2 = sequences[j];
            int len1 = (int) s1.size(), len2 = (int) s2.size();
            _pair_heuristic[i][j].resize(len1 + 1, std::vector<int>(len2 + 1, INF));
            relax(_pair_heuristic[i][j][len1][len2], 0);
            for (int it1 = len1; it1 >= 0; it1--)
                for (int it2 = len2; it2 >= 0; it2--) {
                    if (it1 + 1 <= len1 && it2 + 1 <= len2)
                        relax(_pair_heuristic[i][j][it1][it2],
                              _pair_heuristic[i][j][it1 + 1][it2 + 1] + mtx.at(s1[it1]).at(s2[it2]));
                    if (it1 + 1 <= len1)
                        relax(_pair_heuristic[i][j][it1][it2],
                              _pair_heuristic[i][j][it1 + 1][it2] + mtx.at(s1[it1]).at(GAP));
                    if (it2 + 1 <= len2)
                        relax(_pair_heuristic[i][j][it1][it2],
                              _pair_heuristic[i][j][it1][it2 + 1] + mtx.at(GAP).at(s2[it2]));
                }
        }
    }
}

int HeuristicCalculator::calculate_heuristic(const Node &node) {
    int h = 0;
    for (int i = 0; i < (int) node.size(); i++)
        for (int j = 0; j < i; j++)
            h += _pair_heuristic[i][j][node[i]][node[j]];
    return h;
}
