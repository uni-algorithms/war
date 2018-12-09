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

TEST_CASE("make_columns_great_again") {
    vector<vector<int>> miss_column = {{1, 2},
                                       {3, 4},
                                       {9, 9}};

    vector<vector<int>> miss_row = {{1, 2, 9},
                                    {3, 4, 9}};


    vector<vector<int>> expected = {{1, 2, 9},
                                    {3, 4, 9},
                                    {9, 9, 9}};


    make_columns_great_again(miss_column, 9);
    make_columns_great_again(miss_row, 9);

    REQUIRE(miss_column == expected);
    REQUIRE(miss_row == miss_row);
}

TEST_CASE("make_positive") {
    vector<vector<int>> negative = {{1, 2},
                                    {3, -2},
                                    {7, 7}};

    vector<vector<int>> positive = {{2, 2},
                                    {2, 0},
                                    {2, 2}};

    vector<vector<int>> expected = {{2, 2},
                                    {2, 0},
                                    {2, 2}};

    REQUIRE(make_positive(negative).first.first == -2);
    REQUIRE(negative == expected);

    REQUIRE(make_positive(positive).first.first == 0);
    REQUIRE(positive == expected);
}

TEST_CASE("hungarian easy") {

    vector<pair<vector<vector<int>>, vector<assignment>>> tests = {
            {{{9,  11, 14, 11, 7},
                     {6,  15, 13, 13, 10},
                     {12, 13, 6,  8,  8},
                     {11, 9,  10, 12, 9},
                     {7,  12, 14, 10, 14}},                     {{0, 4}, {1, 0}, {4, 3}, {3, 1}, {2, 2}}},
            {{{5,  3,  2,  8},
                     {7,  9,  2,  6},
                     {6,  4,  5,  7},
                     {5,  7,  7,  8}},                          {{3, 0}, {0, 1}, {1, 2}, {2, 3}}},
            {{{39, 85, 93, 20, 24, 49, 44, 92, 13, 50},
                     {9,  72, 32, 43, 62, 42, 91, 37, 71, 18},
                     {22, 5,  41, 21, 88, 87, 17, 24, 77, 20},
                     {75, 90, 41, 25, 63, 66, 4,  85, 15, 9},
                     {79, 50, 82, 58, 29, 71, 41, 52, 3,  7},
                     {13, 92, 81, 26, 19, 74, 39, 81, 35, 47},
                     {4,  83, 71, 93, 25, 59, 21, 61, 65, 68},
                     {35, 81, 1,  52, 82, 35, 62, 49, 50, 33},
                     {82, 86, 99, 11, 61, 97, 89, 48, 55, 25},
                     {12, 24, 75, 29, 61, 15, 31, 63, 89, 21}}, {{0, 8}, {1, 7}, {2, 1}, {3, 6}, {4, 9}, {5, 4}, {6, 0}, {7, 2}, {8, 3}, {9, 5}}},
            {{{98, 31, 86, 59, 10, 93, 44, 34, 63, 54},
                     {52, 19, 71, 4,  14, 37, 58, 59, 94, 49},
                     {23, 85, 31, 65, 24, 33, 75, 97, 58, 24},
                     {77, 11, 50, 38, 37, 46, 75, 46, 40, 13},
                     {44, 77, 64, 63, 19, 14, 10, 75, 64, 72},
                     {84, 56, 33, 78, 60, 20, 65, 9,  96, 50},
                     {25, 40, 10, 89, 86, 13, 74, 65, 84, 90},
                     {90, 1,  87, 2,  80, 53, 5,  61, 54, 96},
                     {28, 99, 89, 82, 30, 99, 84, 16, 34, 93},
                     {40, 20, 65, 93, 79, 60, 9,  84, 44, 83}}, {{0, 4}, {1, 3}, {2, 0}, {3, 9}, {4, 5}, {5, 7}, {6, 2}, {7, 1}, {8, 8}, {9, 6}}},
            {{{82, 63, 36, 47, 21, 9,  77, 39, 75, 57},
                     {49, 77, 16, 38, 66, 97, 78, 53, 51, 62},
                     {88, 23, 64, 93, 96, 98, 20, 89, 54, 90},
                     {99, 89, 65, 91, 23, 8,  5,  27, 11, 31},
                     {74, 61, 43, 58, 48, 25, 21, 33, 9,  10},
                     {41, 27, 15, 15, 96, 98, 13, 98, 53, 86},
                     {88, 81, 98, 67, 27, 22, 3,  54, 32, 5},
                     {4,  10, 46, 20, 14, 49, 48, 28, 40, 4},
                     {7,  80, 52, 37, 3,  48, 7,  11, 16, 56},
                     {74, 45, 58, 27, 84, 44, 1,  65, 46, 10}}, {{0, 5}, {1, 2}, {2, 1}, {3, 7}, {4, 8}, {5, 3}, {6, 9}, {7, 0}, {8, 4}, {9, 6}}},
    };

    for (int i = 0; i < tests.size(); ++i) {
        const auto test = tests[i];
        WHEN ("solve : " + to_string(i)) {
            vector<assignment> expected = test.second;
            vector<vector<int>> matrix = test.first;
            vector<assignment> actual;
            hungarian(matrix, back_inserter(actual));

            sort(begin(actual), end(actual));
            sort(begin(expected), end(expected));
            REQUIRE(expected == actual);
        }
    }
}

TEST_CASE("invetend scenarios") {

    vector<pair<std::string, pair<int, vector<int>>>> tests = {
            {" 3 2\n"
             " 0 0\n"
             " 5 8\n"
             "11 8\n"
             " 2 5\n"
             " 7 1\n"
             " 5 2",  {49, {0, 0, 1}}},
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
             "1 1",   {16, {0, 0, 1}}},
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
            {" 3  2\n"
             "-3  2\n"
             "-3  0\n"
             "-3 -2\n"
             " 3  0\n"
             " 3  2\n"
             " 0  0", {27, {1, 1, 1}}},
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

TEST_CASE("provided tests") {
    vector<pair<std::string, int>> tests = {
            {"input0.txt",  10928},
            {"input1.txt",  34620},
            {"input2.txt",  103040},
            {"input3.txt",  171711},
            {"input4.txt",  228478},
            {"input5.txt",  416688},
            {"input6.txt",  850720},
            {"input7.txt",  61209},
            {"input8.txt",  220809},
            {"input9.txt",  192942},
            {"input10.txt", 366158},
            {"input11.txt", 16876},
            {"input12.txt", 64240},
            {"input13.txt", 198780},
            {"input14.txt", 644362},
            {"input15.txt", 1600000},
            {"input16.txt", 303960},
            {"input17.txt", 605546},
          //{"input18.txt", 1276961},
          //{"input19.txt", 1720243},
    };

    for (const auto &test : tests) {
        WHEN (test.first) {
            ifstream input(test.first);
            assert(input);
            vector<int> actual_who_what;
            const int actual_min = min_time(input, back_inserter(actual_who_what));
            input.close();
            REQUIRE (actual_min == test.second);
        }
    }
}