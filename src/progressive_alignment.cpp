#include "progressive_alignment.h"
#include "utils.h"
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>

struct SequenceWithIndex {
    Sequence seq;
    int index;

    SequenceWithIndex(Sequence seq = {}, int index = 0) :
            seq(std::move(seq)), index(index) {}
};

typedef std::vector<SequenceWithIndex> SequencesWithIndexes;

std::pair<SequencesWithIndexes, int> align_two_clusters(const SequencesWithIndexes &c1,
                                                        const SequencesWithIndexes &c2, const ScoreMatrix &mtx) {
    std::vector<std::vector<int>> min_score(c1[0].seq.size() + 1, std::vector<int>(c2[0].seq.size() + 1, INF));
    std::vector<std::vector<std::pair<int, int>>> parent(c1[0].seq.size() + 1,
                                                         std::vector<std::pair<int, int>>(c2[0].seq.size() + 1,
                                                                 {-1, -1}));
    min_score[0][0] = 0;
    for (int i = 0; i <= (int)c1[0].seq.size(); i++)
        for (int j = 0; j <= (int)c2[0].seq.size(); j++) {
            for (std::pair<int, int> d : std::vector<std::pair<int, int>>({{1, 0}, {0, 1}, {1, 1}})) {
                int ni = i - d.first, nj = j - d.second;
                if (ni < 0 || nj < 0)
                    continue;
                int score = 0;
                for (const SequenceWithIndex &s1 : c1)
                    for (const SequenceWithIndex &s2 : c2)
                        score += mtx.at((d.first == 0) ? GAP : s1.seq[ni]).at((d.second == 0) ? GAP : s2.seq[nj]);
                if (min_score[i][j] > min_score[ni][nj] + score) {
                    min_score[i][j] = min_score[ni][nj] + score;
                    parent[i][j] = {ni, nj};
                }
            }
        }
    int i = (int)c1[0].seq.size(), j = (int)c2[0].seq.size();
    SequencesWithIndexes aligned_c1((int)c1.size()), aligned_c2((int)c2.size());
    for (int i = 0; i < (int)c1.size(); i++)
        aligned_c1[i] = SequenceWithIndex({}, c1[i].index);
    for (int i = 0; i < (int)c2.size(); i++)
        aligned_c2[i] = SequenceWithIndex({}, c2[i].index);
    while (parent[i][j].first != -1 && parent[i][j].second != -1) {
        int ni = parent[i][j].first, nj = parent[i][j].second;
        for (int id = 0; id < (int)c1.size(); id++)
            aligned_c1[id].seq.push_back((ni == i) ? GAP : c1[id].seq[ni]);
        for (int id = 0; id < (int)c2.size(); id++)
            aligned_c2[id].seq.push_back((nj == j) ? GAP : c2[id].seq[nj]);
        i = ni, j = nj;
    }
    SequencesWithIndexes merged_cluster;
    for (auto & c : aligned_c1) {
        reverse(c.seq.begin(), c.seq.end());
        merged_cluster.push_back(c);
    }
    for (auto & c : aligned_c2) {
        reverse(c.seq.begin(), c.seq.end());
        merged_cluster.push_back(c);
    }
    return {merged_cluster, min_score[(int)c1[0].seq.size()][(int)c2[0].seq.size()]};
}

void align_closest_clusters(std::vector<SequencesWithIndexes> &clusters, const ScoreMatrix &mtx) {
    SequencesWithIndexes alignment;
    int min_score = INF, closest_i = -1, closest_j = -1;
    for (int i = 0; i < (int) clusters.size(); i++)
        for (int j = 0; j < i; j++) {
            std::pair<SequencesWithIndexes, int> a = align_two_clusters(clusters[i], clusters[j], mtx);
            if (a.second <= min_score) {
                min_score = a.second, alignment = a.first;
                closest_i = i, closest_j = j;
            }
        }
    clusters.erase(clusters.begin() + closest_i);
    clusters.erase(clusters.begin() + closest_j);
    clusters.push_back(alignment);
}

AlignmentOutput progressive_alignment(const Sequences &sequences, const ScoreMatrix &mtx) {
    std::vector<SequencesWithIndexes> clusters(sequences.size());
    for (int i = 0; i < (int) sequences.size(); i++)
        clusters[i] = {SequenceWithIndex(sequences[i], i)};
    while ((int) clusters.size() > 1)
        align_closest_clusters(clusters, mtx);
    AlignmentOutput res(sequences.size());
    for (const SequenceWithIndex &s : clusters[0])
        res[s.index] = s.seq;
    return res;
}

