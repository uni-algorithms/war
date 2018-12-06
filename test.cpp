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