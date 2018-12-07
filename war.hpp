#ifndef WAR_HPP
#define WAR_HPP

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

struct coordinates {
    int x, y;
};

void make_square(vector<vector<int>> &rows, const int value) {
    const auto n_r = rows.size();
    const auto n_c = begin(rows)->size();

    if (n_c < n_r) {
        const auto diff = n_r - n_c;
        for (auto &row : rows) {
            fill_n(back_inserter(row), diff, value);
        }
    }

    if (n_r < n_c) {
        const auto diff = n_c - n_r;
        vector<int> empty(n_c, value);
        fill_n(back_inserter(rows), diff, empty);
    }
}

int make_positive(vector<vector<int>> &rows) {
    int minimum = *begin(*begin(rows));
    for (const auto &row : rows) {
        minimum = min(minimum, *min_element(begin(row), end(row)));
    }

    if (minimum < 0) {
        for (auto &row : rows) {
            transform(begin(row), end(row), begin(row), [&minimum](const int &el) { return el - minimum; });
        }
    }

    return minimum;
}

template<typename InputIterator, typename OutputIterator>
void subtract(InputIterator a_first, InputIterator a_last, InputIterator b_first, OutputIterator out) {
    for (; a_first != a_last; ++a_first, ++b_first) {
        out++ = *a_first - *b_first;
    }
}

template<typename OutputIterator>
void pick_coordinates(istream &in, unsigned long n, OutputIterator out) {
    for (auto i = 0; i < n; ++i) {
        int x, y;
        in >> x >> y;
        out++ = {x, y};
    }
}

template<typename OutputIterator>
int one_soldier_solver(const vector<int> &dist_tc, const vector<vector<int>> &adv, OutputIterator who_what) {
    fill_n(who_what, dist_tc.size(), 0);
    const vector<int> dist_sc = *begin(adv);
    const auto min = *min_element(begin(dist_sc), end(dist_sc));
    return accumulate(begin(dist_tc), end(dist_tc), 0) * 2 + min;
}

template<typename OutputIterator>
void advantages(const vector<int> &dist_tc, const vector<vector<int>> &dist_ssc, OutputIterator adv) {
    for (const auto &dist_sc : dist_ssc) {
        vector<int> sub;
        subtract(begin(dist_sc), end(dist_sc), begin(dist_tc), back_inserter(sub));
        adv++ = sub;
    }
}

function<int(const coordinates)> distance(const coordinates &a) {
    return [&](const coordinates &b) {
        return abs(a.x - b.x) + abs(a.y - b.y);
    };
}

template<typename OutputIteratorA, typename OutputIteratorB>
void distances_from_stream(istream &in, unsigned long c, unsigned long s, OutputIteratorA dist_tc,
                           OutputIteratorB dist_ssc) {
    vector<coordinates> components;
    vector<coordinates> soldiers;

    pick_coordinates(in, c, back_inserter(components));
    pick_coordinates(in, s, back_inserter(soldiers));

    int x, y;
    in >> x >> y;
    coordinates target = {x, y};

    transform(begin(components), end(components), dist_tc, distance(target));

    for (const auto &soldier : soldiers) {
        vector<int> dist_sc;
        transform(begin(components), end(components), back_inserter(dist_sc), distance(soldier));
        dist_ssc++ = dist_sc;
    }
}

void minimize_rows(vector<vector<int>> &rows) {
    for (auto &row : rows) {
        int min = *min_element(begin(row), end(row));
        transform(begin(row), end(row), begin(row), [&min](const int &el) { return el - min; });
    }
}

void minimize_columns(vector<vector<int>> &rows) {
    vector<int> min_columns = *begin(rows);

    for (int i = 0; i < min_columns.size(); ++i) {
        for (const auto &row : rows) {
            min_columns[i] = min(row[i], min_columns[i]);
        }

        for (auto &row : rows) {
            row[i] -= min_columns[i];
        }
    }
}

bool between(const long &x, const long &first, const long &last) {
    return first <= x && x < last;
}

template<typename OutputIterator>
void
row_scanning(vector<vector<int>> &rows, unsigned long n, const vector<bool> &cov_rows, vector<bool> &cov_cols,
             OutputIterator out) {
    for (int r = 0; r < n; ++r) {
        if (cov_rows[r]) continue;

        pair<int, pair<int, int>> count = {0, {-1, -1}};
        for (int c = 0; c < n; ++c) {
            if (cov_cols[c]) continue;

            if (rows[r][c] == 0) {
                count = {count.first + 1, {r, c}};
            }
        }

        if (count.first == 1) {
            cov_cols[count.second.second] = true;
            out++ = count.second;
        }
    }
}

template<typename OutputIterator>
void
column_scanning(vector<vector<int>> &rows, unsigned long n, vector<bool> &cov_rows, const vector<bool> &cov_cols,
                OutputIterator out) {
    // column scanning
    for (int c = 0; c < n; ++c) {
        if (cov_cols[c]) continue;

        pair<int, pair<int, int>> count = {0, {-1, -1}};
        for (int r = 0; r < n; ++r) {
            if (cov_rows[r]) continue;

            if (rows[r][c] == 0) {
                count = {count.first + 1, {r, c}};
            }
        }

        if (count.first == 1) {
            cov_rows[count.second.first] = true;
            out++ = count.second;
        }
    }
}

/*
 * If the minimum uncovered == 0
 * then there're at least 4 0s uncovered
 * we have to chose those which are diagonally opposite
 */
template<typename OutputIterator>
void diagonal_scanning(vector<vector<int>> &rows, unsigned long n, vector<bool> &cov_rows, vector<bool> &cov_cols,
                       OutputIterator out) {
    for (int r = 0; r < n; ++r) {
        if (cov_rows[r]) continue;

        for (int c = 0; c < n; ++c) {
            if (cov_rows[r]) continue;
            if (cov_cols[c]) continue;
            if (rows[r][c] != 0) continue;

            int up = r - 1;
            int down = r + 1;

            int left = c - 1;
            int right = c + 1;

            /* in order to be checkable must be:
             * - between [0, n)
             * - not covered
             */
            bool checkable_up = between(up, 0, n) && !cov_rows[up];
            bool checkable_down = between(down, 0, n) && !cov_rows[down];
            bool checkable_left = between(left, 0, n) && !cov_cols[left];
            bool checkable_right = between(right, 0, n) && !cov_cols[right];

            if ((checkable_up && checkable_left && rows[up][left] == 0)
                || (checkable_up && checkable_right && rows[up][right] == 0)
                || (checkable_down && checkable_left && rows[down][left] == 0)
                || (checkable_down && checkable_right && rows[down][right] == 0)) {
                cov_cols[c] = true;
                cov_rows[r] = true;
                out++ = make_pair(r, c);
            }
        }
    }
}

template<typename OutputIterator>
bool cover(vector<vector<int>> &rows, OutputIterator out) {
    const auto n = rows.size();

    vector<pair<int, int>> cov_mat;
    vector<bool> cov_rows(n, false);
    vector<bool> cov_cols(n, false);

    row_scanning(rows, n, cov_rows, cov_cols, back_inserter(cov_mat));

    column_scanning(rows, n, cov_rows, cov_cols, back_inserter(cov_mat));

    if (cov_mat.size() == n) {
        copy(begin(cov_mat), end(cov_mat), out);
        return true;
    }

    // find the uncovered minimum
    int minimum = INT_MAX;
    for (int r = 0; r < n; ++r) {
        if (cov_rows[r]) continue;

        for (int c = 0; c < n; ++c) {
            if (cov_cols[c]) continue;

            minimum = min(minimum, rows[r][c]);
        }
    }

    // if minimum != 0
    // then the matrix can be reduced
    // subtracting the minimum to all the uncovered elements
    // adding the minimum to all the double covered elements
    if (minimum != 0) {
        // reduce
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (!cov_rows[r] && !cov_cols[c]) {
                    rows[r][c] -= minimum;
                }

                if (cov_rows[r] && cov_cols[c]) {
                    rows[r][c] += minimum;
                }
            }
        }

        return cover(rows, out);
    }

    // first chance : already done row and column, now diagonal then row then column

    diagonal_scanning(rows, n, cov_rows, cov_cols, back_inserter(cov_mat));

    row_scanning(rows, n, cov_rows, cov_cols, back_inserter(cov_mat));

    column_scanning(rows, n, cov_rows, cov_cols, back_inserter(cov_mat));

    if (cov_mat.size() == n) {
        copy(begin(cov_mat), end(cov_mat), out);
        return true;
    }

    // last chance : start with diagonal then row then column

    fill(begin(cov_rows), end(cov_rows), false);
    fill(begin(cov_cols), end(cov_cols), false);
    cov_mat.clear();

    diagonal_scanning(rows, n, cov_rows, cov_cols, back_inserter(cov_mat));

    row_scanning(rows, n, cov_rows, cov_cols, back_inserter(cov_mat));

    column_scanning(rows, n, cov_rows, cov_cols, back_inserter(cov_mat));

    if (cov_mat.size() == n) {
        copy(begin(cov_mat), end(cov_mat), out);
        return true;
    }

    return false;
}

template<typename K, typename V>
K take_first(const pair<K, V> &p) {
    return p.first;
}

template<typename K, typename V>
bool compare_second(const pair<K, V> &a, const pair<K, V> &b) {
    return a.second < b.second;
}

void print(const vector<vector<int>> &rows) {
    for (const auto &row : rows) {
        for (const auto &el : row) {
            cout << el << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void print(const vector<pair<int, int>> &pairs) {
    for (const auto &pair : pairs) {
        cout << "(" << pair.first << ", " << pair.second << ") ";
    }
    cout << endl;
}

template<typename OutputIterator>
int min_time(istream &in, OutputIterator who_what) {
    unsigned long c;
    unsigned long s;
    in >> c >> s;

    vector<int> dist_tc;
    vector<vector<int>> dist_ssc;
    distances_from_stream(in, c, s, back_inserter(dist_tc), back_inserter(dist_ssc));
    vector<vector<int>> adv;
    advantages(dist_tc, dist_ssc, back_inserter(adv));

    print(dist_ssc);

    print(adv);

    if (adv.size() == 1) {
        return one_soldier_solver(dist_tc, adv, who_what);
    }

    vector<vector<int>> clone(adv);

    const auto min_adv = make_positive(clone);

    print(clone);

    make_square(clone, abs(min_adv));

    print(clone);

    minimize_rows(clone);

    print(clone);

    minimize_columns(clone);

    print(clone);

    vector<pair<int, int>> indexes;
    cover(clone, back_inserter(indexes));

    print(indexes);

    // sort indexes by component
    sort(begin(indexes), end(indexes), compare_second<int, int>);

    // remove non-existent components
    indexes.resize(c);

    print(indexes);

    // init minimums
    int min_soldier = -1;
    int min_chosen_adv = INT_MAX;

    // find minimums
    for (const auto &index : indexes) {
        const auto soldier = index.first;
        const auto component = index.second;

        if (!between(soldier, 0, s)) continue;

        const int candidate_adv = adv[soldier][component];

        if (candidate_adv < min_chosen_adv) {
            min_chosen_adv = candidate_adv;
            min_soldier = soldier;
        }
    }

    // total_adv is the sum of the advantages of the not useless soldier
    int total_adv = 0;

    // change every useless soldier to the min_soldier
    // a soldier is useless <=> is not the min_soldier && (the adv >= 0 || does not belong to the matrix)
    for (auto &index : indexes) {
        const auto soldier = index.first;
        const auto component = index.second;

        if (!between(soldier, 0, s) || adv[soldier][component] >= 0) {
            index.first = min_soldier;
            continue;
        }

        total_adv += adv[soldier][component];
        cout << adv[soldier][component] << " + ";
    }

    cout << endl;

    total_adv = min_chosen_adv >= 0 ? min_chosen_adv : total_adv;
    transform(begin(indexes), end(indexes), who_what, take_first<int, int>);
    return accumulate(begin(dist_tc), end(dist_tc), 0) * 2 + total_adv;
}

#endif //WAR_HPP