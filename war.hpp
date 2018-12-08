#ifndef WAR_HPP
#define WAR_HPP

#include "hungarian.hpp"

using namespace std;

struct coordinates {
    int x, y;
};

bool make_columns_great_again(vector<vector<int>> &rows, const int value) {
    const auto nr = rows.size();
    const auto nc = begin(rows)->size();

    if (nc < nr) {
        const auto diff = nr - nc;
        for (auto &row : rows) {
            fill_n(back_inserter(row), diff, value);
        }
    }

    return nc < nr;
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

bool between(const long &x, const long &first, const long &last) {
    return first <= x && x < last;
}

bool compare_columns(const node &a, const node &b) {
    return a.column() < b.column();
}

pair<int, int> find_min_soldier(const vector<node> &indexes, const vector<vector<int>> &adv, unsigned long c) {
    int min_soldier = -1;
    int min_chosen_adv = INT_MAX;

    // find min_soldier
    for (const auto &node : indexes) {
        const auto soldier = node.row();
        const auto component = node.column();

        if (!between(component, 0, c)) continue;

        const int candidate_adv = adv[soldier][component];

        if (candidate_adv < min_chosen_adv) {
            min_chosen_adv = candidate_adv;
            min_soldier = soldier;
        }
    }

    return {min_soldier, min_chosen_adv};
}

// a soldier is useless if either
// - 0 < adv
// - the assigned component is dummy
bool is_useless(const node &n, const vector<vector<int>> &adv, unsigned long c) {
    return !between(n.column(), 0, c) || 0 <= adv[n.row()][n.column()];
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

    if (adv.size() == 1) {
        return one_soldier_solver(dist_tc, adv, who_what);
    }

    vector<vector<int>> clone(adv);

    make_positive(clone);

    // if there are more soldiers than components
    // => adds dummy components with adv = 0
    const bool add_dummy = make_columns_great_again(clone, 0);

    vector<node> indexes;
    hungarian(clone, back_inserter(indexes));

    // sort indexes by component
    sort(begin(indexes), end(indexes), compare_columns);

    // remove dummy components
    if (add_dummy)
        indexes.resize(c);

    const auto min_pair = find_min_soldier(indexes, adv, c);
    const auto min_soldier = min_pair.first;
    const auto min_chosen_adv = min_pair.second;

    const auto target_component = accumulate(begin(dist_tc), end(dist_tc), 0) * 2;

    // all the soldiers are useless
    if (min_chosen_adv >= 0) {
        fill_n(who_what, dist_tc.size(), 0);
        return target_component + min_chosen_adv;
    }

    // total_adv is the sum of the advantages of the non-useless soldiers
    int total_adv = 0;

    for (const auto &n : indexes) {
        if (is_useless(n, adv, c)) continue;
        total_adv += adv[n.row()][n.column()];
    }

    const int components_not_taken = c - indexes.size();
    transform(begin(indexes), end(indexes), who_what, [&](const node &n) { return is_useless(n, adv, c) ? min_soldier : n.row(); });
    fill_n(who_what, max(0, components_not_taken), min_soldier);
    return target_component + total_adv;
}

#endif //WAR_HPP