// hungarian method from https://github.com/t3nsor/codebook/blob/master/bipartite-mincost.cpp

#ifndef WAR_HUNGARIAN_HPP
#define WAR_HUNGARIAN_HPP

#include <iterator>
#include <algorithm>
#include <fstream>
#include <functional>
#include <numeric>
#include <ostream>
#include <vector>
#include <limits.h>
#include <iostream>

using namespace std;

class assignment {
    int r, c;

public:
    assignment() = default;

    assignment(int r, int c) : r(r), c(c) {}

    int row() const { return r; }

    int column() const { return c; }

    bool operator==(const assignment &rhs) const {
        return r == rhs.r &&
               c == rhs.c;
    }

    bool operator!=(const assignment &rhs) const {
        return !(rhs == *this);
    }
};

// Min cost bipartite matching via shortest augmenting paths
//
// This is an O(n^3) implementation of a shortest augmenting path
// algorithm for finding min cost perfect matchings in dense
// graphs.  In practice, it solves 1000x1000 problems in around 1
// second.
//
//   cost[i][j] = cost for pairing left node i with right node j
//   Lmate[i] = index of right node that left node i pairs with
//   Rmate[j] = index of left node that right node j pairs with
//
// The values in cost[i][j] may be positive or negative.  To perform
// maximization, simply negate the cost[][] matrix.

typedef vector<int> VI;
typedef vector<VI> VVI;

template<typename OutputIterator>
void hungarian(const VVI &cost, OutputIterator out) {
    const auto n = cost.size();

    // construct dual feasible solution
    VI u(n);
    VI v(n);

    for (int i = 0; i < n; i++) {
        u[i] = cost[i][0];
        for (int j = 1; j < n; j++) u[i] = min(u[i], cost[i][j]);
    }
    for (int j = 0; j < n; j++) {
        v[j] = cost[0][j] - u[0];
        for (int i = 1; i < n; i++) v[j] = min(v[j], cost[i][j] - u[i]);
    }

    // construct primal solution satisfying complementary slackness
    VI Lmate = VI(n, -1);
    VI Rmate = VI(n, -1);
    int mated = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (Rmate[j] != -1) continue;
            if (abs(cost[i][j] - u[i] - v[j]) < 1e-10) {
                Lmate[i] = j;
                Rmate[j] = i;
                mated++;
                break;
            }
        }
    }

    VI dist(n);
    VI dad(n);
    vector<bool> seen(n);

    // repeat until primal solution is feasible
    while (mated < n) {

        // find an unmatched left node
        int s = 0;
        while (Lmate[s] != -1) s++;

        // initialize Dijkstra
        fill(dad.begin(), dad.end(), -1);
        fill(seen.begin(), seen.end(), 0);
        for (int k = 0; k < n; k++)
            dist[k] = cost[s][k] - u[s] - v[k];

        int j = 0;
        while (true) {

            // find closest
            j = -1;
            for (int k = 0; k < n; k++) {
                if (seen[k]) continue;
                if (j == -1 || dist[k] < dist[j]) j = k;
            }
            seen[j] = true;

            // termination condition
            if (Rmate[j] == -1) break;

            // relax neighbors
            const int i = Rmate[j];
            for (int k = 0; k < n; k++) {
                if (seen[k]) continue;
                const int new_dist = dist[j] + cost[i][k] - u[i] - v[k];
                if (dist[k] > new_dist) {
                    dist[k] = new_dist;
                    dad[k] = j;
                }
            }
        }

        // update dual variables
        for (int k = 0; k < n; k++) {
            if (k == j || !seen[k]) continue;
            const int i = Rmate[k];
            v[k] += dist[k] - dist[j];
            u[i] -= dist[k] - dist[j];
        }
        u[s] += dist[j];

        // augment along path
        while (dad[j] >= 0) {
            const int d = dad[j];
            Rmate[j] = Rmate[d];
            Lmate[Rmate[j]] = j;
            j = d;
        }
        Rmate[j] = s;
        Lmate[s] = j;

        mated++;
    }

    for (int i = 0; i < n; i++) {
        out++ = assignment(Rmate[i], i);
    }
}


#endif //WAR_HUNGARIAN_HPP
