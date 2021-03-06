#ifndef WAR_HPP
#define WAR_HPP

#include "hungarian.hpp"

using namespace std;

struct coordinates {
    int x, y;
};

bool make_square(vector<vector<int>> &rows, const int value) {
    const auto nr = rows.size();
    const auto nc = begin(rows)->size();

    if (nc < nr) {
        const auto diff = nr - nc;
        for (auto &row : rows) {
            fill_n(back_inserter(row), diff, value);
        }
    }

    if (nr < nc) {
        const auto diff = nc - nr;
        const vector<int> empty(nc, value);
        for (int i = 0; i < diff; ++i) {
            rows.push_back(empty);
        }
    }

    return nc < nr;
}

pair<pair<int, int>, pair<int, int>> make_positive(vector<vector<int>> &rows) {
    int minimum = *begin(*begin(rows));
    int minimum_s = 0;
    int maximum = *begin(*begin(rows));
    int maximum_s = 0;
    for (int s = 0; s < rows.size(); ++s) {
        for (const auto &el : rows[s]) {
            if (el < minimum) {
                minimum = el;
                minimum_s = s;
            }

            if (el > maximum) {
                maximum = el;
                maximum_s = s;
            }
        }
    }

    if (minimum < 0) {
        for (auto &row : rows) {
            transform(begin(row), end(row), begin(row), [&minimum, &maximum](const int &el) { return abs(minimum) + min(0, el); });
        }
    }

    return {{minimum, minimum_s}, {maximum, maximum_s}};
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

bool compare_columns(const assignment &a, const assignment &b) {
    return a.column() < b.column();
}

bool compare_rows(const assignment &a, const assignment &b) {
    return a.row() < b.row();
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

    const auto min_max = make_positive(clone);

    if (c == s) {
        hungarian(clone, back_inserter(as));
    } else if (c > s) {
        // c > s
        // c == s
        // there are some components which will be taken from the target
        make_square(clone, abs(min_max.second.first));

        hungarian(clone, back_inserter(as));

        // sort indexes by soldier
        // => all the dummy soldier will be at the end
        sort(begin(as), end(as), compare_rows);

        // remove dummy soldiers
        as.resize(s);
    } else {
        // c < s
        // some soldiers will not be taken
        // add dummy components with max adv (necessary for the correct work of the HA)
        make_square(clone, abs(min_max.second.first));

        hungarian(clone, back_inserter(as));

        // sort indexes by component
        // => all the dummy components will be at the end
        sort(begin(as), end(as), compare_columns);

        // remove dummy components
        as.resize(c);
    }

    bool exists_useless_soldier = false;
    int min_soldier = -1;
    int min_advantage = INT_MAX;
    for (const auto &a : as) {
        const int advantage = adv[a.row()][a.column()];
        if (advantage > 0) {
            exists_useless_soldier = true;
        }

        if (advantage < min_advantage) {
            min_soldier = a.row();
            min_advantage = advantage;
        }
    }

    if (!exists_useless_soldier) {
        // all soldier are useful
        // there is no need to find the min_soldier

        int total_adv = 0;
        vector<int> what_who(c, 0);
        for (const auto &a : as) {
            const int advantage = adv[a.row()][a.column()];
            total_adv += advantage;
            what_who[a.column()] = a.row();
        }

        copy(begin(what_who), end(what_who), out);
        return target_component + total_adv;
    }

    // there are some soldier useless
    // find the min_soldier
    // if min_soldier is useless -> all soldiers are useless

    bool exists_useful_soldier = min_advantage < 0;

    if (!exists_useful_soldier) {
        // all soldiers are useless
        // then the total_adv = to the min(adv)

        fill_n(out, c, min_max.first.second);
        return target_component + min_max.first.first;
    }

    // there are some useful soldiers
    // skip all the useless soldiers

    int total_adv = 0;
    vector<int> what_who(c, min_soldier);
    for (const auto &a : as) {
        if (adv[a.row()][a.column()] >= 0) continue;

        const int advantage = adv[a.row()][a.column()];
        total_adv += advantage;
        what_who[a.column()] = a.row();
    }
    copy(begin(what_who), end(what_who), out);
    return target_component + total_adv;
}

#endif //WAR_HPP