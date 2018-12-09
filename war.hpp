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

bool compare_columns(const assignment &a, const assignment &b) {
    return a.column() < b.column();
}

pair<int, int> find_min_soldier(const vector<assignment> &indexes, const vector<vector<int>> &adv, unsigned long c) {
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
bool is_useless(const assignment &n, const vector<vector<int>> &adv, unsigned long c) {
    return !between(n.column(), 0, c) || 0 <= adv[n.row()][n.column()];
}

template<typename OutputIterator>
int min_time(istream &in, OutputIterator out) {
    unsigned long c;
    unsigned long s;
    in >> c >> s;

    vector<int> dist_tc;
    vector<vector<int>> dist_ssc;
    distances_from_stream(in, c, s, back_inserter(dist_tc), back_inserter(dist_ssc));
    vector<vector<int>> adv;
    advantages(dist_tc, dist_ssc, back_inserter(adv));

    const auto target_component = accumulate(begin(dist_tc), end(dist_tc), 0) * 2;
    vector<vector<int>> clone(adv);
    vector<assignment> as;

    if (c >= s) {
        // c > s
        // c == s
        // there are some components which will be taken from the target
        // there is no need to add dummy components

        make_positive(clone);
        hungarian(clone, back_inserter(as));
    } else {
        // c < s
        // some soldiers will not be taken
        // add dummy components with 0 adv (necessary for the correct work of the HA)

        const int fattore = make_positive(clone);

        // add dummies
        make_columns_great_again(clone, abs(fattore));


        hungarian(clone, back_inserter(as));

        // sort indexes by component
        // => all the dummy components will be at the end
        sort(begin(as), end(as), compare_columns);

        // remove dummy components
        as.resize(c);
    }

    bool exists_useless_soldier = false;
    for (const auto &a : as) {
        const int advantage = adv[a.row()][a.column()];
        if (advantage > 0) {
            exists_useless_soldier = true;
        }
    }

    if (!exists_useless_soldier) {
        // all soldier are useful
        // there is no need to find the min_soldier

        int total_adv = 0;
        for (const auto &a : as) {
            const int advantage = adv[a.row()][a.column()];
            total_adv += advantage;
        }

        int min_soldier = 0;

        vector<int> what_who(c, min_soldier);
        for (const auto &a : as) {
            what_who[a.column()] = a.row();
        }

        copy(begin(what_who), end(what_who), out);
        return target_component + total_adv;
    }

    // there are some soldier useless
    // find the min_soldier
    // if min_soldier is useless -> all soldiers are useless

    int min_soldier = -1;
    int min_advantage = INT_MAX;
    for (const auto &a : as) {
        const int advantage = adv[a.row()][a.column()];
        if (advantage < min_advantage) {
            min_soldier = a.row();
            min_advantage = advantage;
        }
    }

    bool exists_useful_soldier = min_advantage < 0;

    if (!exists_useful_soldier) {
        // all soldiers are useless
        // there is no need to calculate the total_adv

        vector<int> what_who(c, min_soldier);
        copy(begin(what_who), end(what_who), out);
        return target_component + min_advantage;
    }

    // there are some useful soldiers
    // erase all the useless soldiers

    as.erase(remove_if(begin(as), end(as), [&adv](const assignment &a) { return adv[a.row()][a.column()] >= 0; }));

    // now as contains only useful soldiers

    int total_adv = 0;
    for (const auto &a : as) {
        const int advantage = adv[a.row()][a.column()];
        total_adv += advantage;
    }

    vector<int> what_who(c, min_soldier);
    for (const auto &a : as) {
        what_who[a.column()] = a.row();
    }
    copy(begin(what_who), end(what_who), out);
    return target_component + total_adv;
}

#endif //WAR_HPP