#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include "war.hpp"

TEST_CASE("advantages") {
    vector<int> dist_target_component = {5, 8};
    vector<vector<int>> dist_soldiers_component = {{1, 2},
                                                   {5, 9}};
    vector<vector<int>> expected = {{-4, -6},
                                    {0,  1}};
    vector<vector<int>> actual;
    advantages(dist_target_component, dist_soldiers_component, back_inserter(actual));
    REQUIRE(expected == actual);
}

TEST_CASE("distances_from_stream") {
    stringstream in("2 3\n"
                    "2 0\n"
                    "2 2\n"
                    "5 5\n"
                    "0 6");

    vector<int> expected_tc = {5, 8};
    vector<vector<int>> expected_ssc = {{1, 2},
                                        {5, 8}};

    vector<int> actual_tc;
    vector<vector<int>> actual_ssc;
    distances_from_stream(in, 2, 2, back_inserter(actual_tc), back_inserter(actual_ssc));
    REQUIRE(expected_tc == actual_tc);
    REQUIRE(expected_ssc == actual_ssc);
}

TEST_CASE("make_square") {
    vector<vector<int>> miss_column = {{1, 2},
                                       {3, 4},
                                       {9, 9}};

    vector<vector<int>> miss_row = {{1, 2, 9},
                                    {3, 4, 9}};


    vector<vector<int>> expected = {{1, 2, 9},
                                    {3, 4, 9},
                                    {9, 9, 9}};


    make_square(miss_column, 9);
    make_square(miss_row, 9);

    REQUIRE(miss_column == expected);
    REQUIRE(miss_row == expected);
}

TEST_CASE("make_positive") {
    vector<vector<int>> negative = {{1, 2},
                                    {3, -2},
                                    {7, 7}};

    vector<vector<int>> positive = {{3, 4},
                                    {5, 0},
                                    {9, 9}};

    vector<vector<int>> expected = {{3, 4},
                                    {5, 0},
                                    {9, 9}};

    REQUIRE(make_positive(negative) == -2);
    REQUIRE(negative == expected);

    REQUIRE(make_positive(positive) == 0);
    REQUIRE(positive == expected);
}

TEST_CASE("hungarian easy") {
    vector<vector<int>> matrix = {{9,  11, 14, 11, 7},
                                  {6,  15, 13, 13, 10},
                                  {12, 13, 6,  8,  8},
                                  {11, 9,  10, 12, 9},
                                  {7,  12, 14, 10, 14}};

    vector<pair<int, int>> expected = {{0, 4},
                                       {1, 0},
                                       {4, 3},
                                       {3, 1},
                                       {2, 2}};

    vector<pair<int, int>> actual;
    minimize_rows(matrix);
    minimize_columns(matrix);
    cover(matrix, back_inserter(actual));

    for (const auto &p : actual) {
        cout << p.first << "-" << p.second << "; ";
    }

    REQUIRE(expected == actual);
}

TEST_CASE("hungarian hardcore") {
    vector<vector<int>> matrix = {{5, 3, 2, 8},
                                  {7, 9, 2, 6},
                                  {6, 4, 5, 7},
                                  {5, 7, 7, 8}};

    vector<pair<int, int>> expected = {{3, 0},
                                       {0, 1},
                                       {1, 2},
                                       {2, 3}};

    vector<pair<int, int>> actual;
    minimize_rows(matrix);
    minimize_columns(matrix);
    cover(matrix, back_inserter(actual));

    for (const auto &p : actual) {
        cout << p.first << "-" << p.second << "; ";
    }

    REQUIRE(expected == actual);
}

TEST_CASE("more soldiers") {

    vector<pair<std::string, pair<int, vector<int>>>> tests = {
            {" 3 2\n"
             " 0 0\n"
             " 5 8\n"
             "11 8\n"
             " 2 5\n"
             " 7 1\n"
             " 5 2",  {49, {1, 1, 1}}},
            {" 3  4\n"
             "-2 -4\n"
             " 9  2\n"
             "-2  5\n"
             " 3  6\n"
             " 5 -1\n"
             "-7 -1\n"
             " 0 -6\n"
             " 2  0", {43, {3, 1, 0}}},
            {"3 2\n"
             "1 4\n"
             "3 1\n"
             "6 2\n"
             "1 6\n"
             "6 1\n"
             "1 1",   {16, {0, 1, 1}}},
            {" 4  4\n"
             " 8  5\n"
             "-9  1\n"
             " 7  3\n"
             " 1  6\n"
             "-4  5\n"
             " 8  5\n"
             " 6 -1\n"
             "-1 -8\n"
             "-7 -1", {73, {1, 1, 2, 0}}},
    };

    for (int i = 0; i < tests.size(); ++i) {
        const auto test = tests[i];
        WHEN ("solve : " + to_string(i)) {
            stringstream in(test.first);
            vector<int> actual_who_what;
            const int actual_min = min_time(in, back_inserter(actual_who_what));

            REQUIRE (actual_min == test.second.first);
            REQUIRE (actual_who_what == test.second.second);
        }
    }
}