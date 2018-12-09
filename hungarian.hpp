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
    assignment() {}

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

    bool operator<(const assignment &rhs) const {
        if (r < rhs.r)
            return true;
        if (rhs.r < r)
            return false;
        return c < rhs.c;
    }

    bool operator>(const assignment &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const assignment &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const assignment &rhs) const {
        return !(*this < rhs);
    }

    friend ostream &operator<<(ostream &os, const assignment &n) {
        os << "[" << n.r << ", " << n.c << "]";
        return os;
    }
};

template<typename OutputIterator>
void stars_unstars(const assignment &prime, vector<assignment> &primes, vector<assignment> &stars, OutputIterator to_stars,
                   OutputIterator to_unstars) {

    const auto star_it = find_if(begin(stars), end(stars),
                                 [&prime](const assignment &star) { return prime.column() == star.column(); });

    if (star_it == end(stars)) {
        return;
    }

    const auto star = *star_it;
    const auto to_star = *find_if(begin(primes), end(primes),
                                  [&star](const assignment &prime) { return star.row() == prime.row(); });

    to_stars++ = to_star;
    to_unstars++ = star;

    stars_unstars(to_star, primes, stars, to_stars, to_unstars);
}

void unstars(const assignment &prime, vector<assignment> &primes, vector<assignment> &stars, vector<bool> &cover_row) {

    vector<assignment> to_stars = {prime};
    vector<assignment> to_unstars;

    stars_unstars(prime, primes, stars, back_inserter(to_stars), back_inserter(to_unstars));

    if (to_unstars.size() == stars.size()) {
        stars.clear();
    }

    if (!to_unstars.empty() && !stars.empty()) {
        sort(to_unstars.begin(), to_unstars.end());
        sort(stars.begin(), stars.end());

        vector<assignment> diff;
        set_difference(
                begin(stars), end(stars),
                begin(to_unstars), end(to_unstars),
                back_inserter(diff)
        );
        stars = diff;
    }


    stars.insert(end(stars), begin(to_stars), end(to_stars));
    primes.clear();
    fill(begin(cover_row), end(cover_row), false);
}

bool
find_primes(vector<vector<int>> &rows, unsigned long nr, unsigned long nc, vector<assignment> &stars, vector<assignment> &primes, vector<bool> &cover_col,
            vector<bool> &cover_row) {

    for (int r = 0; r < nr; ++r) {
        for (int c = 0; c < nc; ++c) {
            if (cover_row[r]) break;
            if (cover_col[c]) continue;
            if (rows[r][c] != 0) continue;

            const assignment prime = {r, c};
            primes.push_back(prime);
            const auto star_it = find_if(begin(stars), end(stars), [&r](const assignment &star) { return r == star.row(); });

            if (star_it == end(stars)) {
                unstars(prime, primes, stars, cover_row);
                return false;
            }

            cover_row[r] = true;
            cover_col[star_it->column()] = false;
        }
    }

    return true;
}

void minimize_rows(vector<vector<int>> &rows, unsigned long nc) {
    for (auto &row : rows) {
        int min = *min_element(begin(row), end(row));
        transform(begin(row), end(row), begin(row), [&min](const int el) { return el - min; });
    }
}

template<typename OutputIterator>
void find_stars(vector<vector<int>> &rows, unsigned long nr, unsigned long nc, OutputIterator out) {

    vector<bool> starred_row(nr, false);
    vector<bool> starred_col(nc, false);

    for (int r = 0; r < nr; ++r) {
        for (int c = 0; c < nc; ++c) {
            if (starred_row[r]) break;
            if (starred_col[c]) continue;
            if (rows[r][c] != 0) continue;

            out++ = {r, c};
            starred_row[r] = true;
            starred_col[c] = true;
        }
    }
}

bool
is_done(vector<vector<int>> &rows, unsigned long nr, unsigned long nc, vector<assignment> &stars, vector<bool> &cover_col) {

    for (const auto &star : stars) {
        cover_col[star.column()] = true;
    }

    long covered_col = count_if(begin(cover_col), end(cover_col), [](const bool b) { return b; });
    return covered_col == nr;
}

void minimize_cross(vector<vector<int>> &rows, unsigned long nr, unsigned long nc, const vector<bool> &cover_col,
                    const vector<bool> &cover_row) {
    int minimum = INT_MAX;

    for (int r = 0; r < nr; ++r) {
        for (int c = 0; c < nc; ++c) {
            if (cover_row[r]) break;
            if (cover_col[c]) continue;

            minimum = min(minimum, rows[r][c]);
        }
    }

    for (int r = 0; r < nr; ++r) {
        for (int c = 0; c < nc; ++c) {
            if (!cover_row[r] && !cover_col[c]) {
                rows[r][c] -= minimum;
            }

            if (cover_row[r] && cover_col[c]) {
                rows[r][c] += minimum;
            }
        }
    }
}

template<typename OutputIterator>
void hungarian(vector<vector<int>> &rows, OutputIterator out) {
    const auto nr = rows.size();
    const auto nc = begin(rows)->size();

    vector<bool> cover_row(nr, false);
    vector<bool> cover_col(nc, false);
    vector<assignment> stars;
    vector<assignment> primes;

    minimize_rows(rows, nc);
    find_stars(rows, nr, nc, back_inserter(stars));

    int step = 3;
    while (step != 0) {
        switch (step) {
            case 3: {
                const bool done = is_done(rows, nr, nc, stars, cover_col);
                step = done ? 0 : 4;
                break;
            }
            case 4: {
                const bool which = find_primes(rows, nr, nc, stars, primes, cover_col, cover_row);
                step = which ? 6 : 3;
                break;
            }
            case 6: {
                minimize_cross(rows, nr, nc, cover_col, cover_row);
                step = 4;
                break;
            }
        }
    }

    copy(begin(stars), end(stars), out);
}

#endif //WAR_HUNGARIAN_HPP
