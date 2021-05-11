#include <common.h>

int calculate_alignment_score(const AlignmentOutput &alignment, const ScoreMatrix &mtx) {
    int score = 0;
    for (int pos = 0; pos < (int)alignment[0].size(); pos++)
        for (int i = 0; i < (int)alignment.size(); i++)
            for (int j = 0; j < i; j++)
                score += mtx.at(alignment[i][pos]).at(alignment[j][pos]);
    return score;
}

