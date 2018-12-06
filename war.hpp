#ifndef WAR_HPP
#define WAR_HPP

#include <algorithm>
#include <fstream>
#include <functional>
#include <numeric>
#include <ostream>
#include <vector>

using namespace std;

struct coordinates {
    int x, y;
};

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

int doubles(int acc, const int &x) {
    return acc + x * 2;
}

template<typename OutputIterator>
int one_soldier_solver(const vector<int> &dist_tc, const vector<vector<int>> &adv, OutputIterator who_what) {
    transform(begin(dist_tc), end(dist_tc), who_what, [](const int ignore) { return 0; });
    const vector<int> dist_sc = *begin(adv);
    const auto min = *min_element(begin(dist_sc), end(dist_sc));
    return accumulate(begin(dist_tc), end(dist_tc), min, doubles);
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
void distances_from_stream(istream &in, unsigned long c, unsigned long s, OutputIteratorA dist_tc, OutputIteratorB dist_ssc) {
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

    // TODO
    return -1;
}

#endif //WAR_HPP