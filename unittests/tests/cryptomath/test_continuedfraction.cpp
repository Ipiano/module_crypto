/*! @file */ 
#include "../../catch.hpp"

#include "continuedfractions.h"

#include <cmath>
#include <utility>

#define pi 3.14159265359

using namespace std;
using namespace cryptomath::continuedfractions;

/*!
    \test Tests converting doubles to continued fractions
        - 0
        - 1
        - 23
        - 23.5
        - 23.3333
        - pi
*/
TEST_CASE("Double to continued fraction")
{
    vector<uint64_t> result;

    SECTION("0 to continued fraction")
    {
        result = dtocf(0);

        REQUIRE(result == (vector<uint64_t>{0}));
    };

    SECTION("1 to continued fraction")
    {
        result = dtocf(1);

        REQUIRE(result == (vector<uint64_t>{1}));
    };

    SECTION("23 to continued fraction")
    {
        result = dtocf(23);

        REQUIRE(result == (vector<uint64_t>{23}));
    };

    SECTION("23.5 to continued fraction")
    {
        result = dtocf(23.5);

        REQUIRE(result == (vector<uint64_t>{23, 2}));
    };

    SECTION("23.3333 to continued fraction")
    {
        result = dtocf(23.3333, 3);

        REQUIRE(result == (vector<uint64_t>{23, 3, 3333, 7778288}));
    };

    SECTION("pi to continued fraction")
    {
        result = dtocf(pi, 3);

        REQUIRE(result == (vector<uint64_t>{3, 7, 15, 1}));
    };
};

/*!
    \test Tests converting raionals to continued fractions
        - 10/8
        - 10/28
        - 3/7
        - 13/9
        - 1/1
        - 0/1
*/
TEST_CASE("Fraction to continued fraction")
{
    vector<uint64_t> result;

    SECTION("10/8 to continued fraction")
    {
        result = ftocf(10, 8);

        REQUIRE(result == (vector<uint64_t>{1, 4}));
    };

    SECTION("10/28 to continued fraction")
    {
        result = ftocf(10, 28);

        REQUIRE(result == (vector<uint64_t>{0, 2, 1, 4}));
    };

    SECTION("3/7 to continued fraction")
    {
        result = ftocf(3, 7);

        REQUIRE(result == (vector<uint64_t>{0, 2, 3}));
    };

    SECTION("13/9 to continued fraction")
    {
        result = ftocf(13, 9);

        REQUIRE(result == (vector<uint64_t>{1, 2, 4}));
    };

    SECTION("1/1 to continued fraction")
    {
        result = ftocf(1, 1);

        REQUIRE(result == (vector<uint64_t>{1}));
    };

    SECTION("0/1 to continued fraction")
    {
        result = ftocf(0, 1);

        REQUIRE(result == (vector<uint64_t>{0}));
    };

    SECTION("1/0 to continued fraction")
    {
        REQUIRE_THROWS(result = ftocf(1, 0));
    };
};

/*!
    \test Tests converting simple square roots to continued fractions with repeating portions
        - sqrt(7)
        - sqrt(3)
        - sqrt(0)
        - sqrt(1)
        - sqrt(4)
*/
TEST_CASE("Continued fractions of simple square roots")
{
    vector<uint64_t> result;

    SECTION("sqrt(7) to continued fraction")
    {
        result = sqrttocf(7);

        REQUIRE(result == (vector<uint64_t>{2, 1, 1, 1, 4}));
    };

    SECTION("sqrt(3) to continued fraction")
    {
        result = sqrttocf(3);

        REQUIRE(result == (vector<uint64_t>{1, 1, 2}));
    };

    SECTION("sqrt(0) to continued fraction")
    {
        result = sqrttocf(0);

        REQUIRE(result == (vector<uint64_t>{0}));
    };

    SECTION("sqrt(1) to continued fraction")
    {
        result = sqrttocf(1);

        REQUIRE(result == (vector<uint64_t>{1}));
    };

    SECTION("sqrt(4) to continued fraction")
    {
        result = sqrttocf(4);

        REQUIRE(result == (vector<uint64_t>{2}));
    };
}

/*! 
    \test Tests converting continued fractions to rational approximations
        - 0
        - 1
        - 23
        - 23; 2
        - 23; 3, 3333, 7778288
        - 2; 1, 1, 1, 4
        - 1; 1, 2
        - 3; 7, 15, 1
        - 1; 4
        - 0; 2, 1, 4
        - 0; 2, 3
        - 1; 2, 4
*/
TEST_CASE("Continued fraction to approximation")
{
    vector<pair<uint64_t, uint64_t>> result;

    SECTION("0 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{0});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{0, 1}}));
    };

    SECTION("1 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{1});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{1, 1}}));
    };

    SECTION("23 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{23});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{23, 1}}));
    };

    SECTION("23.5 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{23, 2});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{23, 1}, {47, 2}}));
    };

    SECTION("23.3333 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{23, 3, 3333, 7778288});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{23, 1}, {70, 3}, {233333, 10000}, {1814931273974, 77782880003}}));
    };

    SECTION("sqrt(7) to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{2, 1, 1, 1, 4}, 1);

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{2, 1}, {3, 1}, {5, 2}, {8, 3}, {37, 14}, {45, 17}, {82, 31}, {127, 48}, {590, 223}}));
    };

    SECTION("sqrt(3) to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{1, 1, 2}, 3);

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{1, 1}, {2, 1}, {5, 3}, {7, 4}, {19, 11}, {26, 15}, {71, 41}, {97, 56}, {265, 153}}));
    };

    SECTION("pi to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{3, 7, 15, 1});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{3, 1}, {22, 7}, {333, 106}, {355, 113}}));
    };

    SECTION("10/8 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{1, 4});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{1, 1}, {5, 4}}));
    };

    SECTION("10/28 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{0, 2, 1, 4});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{0, 1}, {1, 2}, {1, 3}, {5, 14}}));
    };

    SECTION("3/7 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{0, 2, 3});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{0, 1}, {1, 2}, {3, 7}}));
    };

    SECTION("13/9 to continued fraction approximation")
    {
        result = cftof(vector<uint64_t>{1, 2, 4});

        REQUIRE(result == (vector<pair<uint64_t, uint64_t>>{{1, 1}, {3, 2}, {13, 9}}));
    };
};

/*! 
    \test Tests converting continued fractions to real approximation
        - 0
        - 1
        - 23
        - 23; 2
        - 23; 3, 3333, 7778288
        - 2; 1, 1, 1, 4
        - 1; 1, 2
        - 3; 7, 15, 1, 292, 1
*/
TEST_CASE("Continued fraction to double")
{
    double result;
    SECTION("0 from continued fraction")
    {
        result = cftod(vector<uint64_t>{0});

        REQUIRE(result == Approx(0));
    };

    SECTION("1 from continued fraction")
    {
        result = cftod(vector<uint64_t>{1});

        REQUIRE(result == Approx(1));
    };

    SECTION("23 from continued fraction")
    {
        result = cftod(vector<uint64_t>{23});

        REQUIRE(result == Approx(23));
    };

    SECTION("23.5 from continued fraction")
    {
        result = cftod(vector<uint64_t>{23, 2});

        REQUIRE(result == Approx(23.5));
    };

    SECTION("23.3333 from continued fraction")
    {
        result = cftod(vector<uint64_t>{23, 3, 3333, 7778288});

        REQUIRE(result == Approx(23.3333));
    };

    SECTION("sqrt(7) from continued fraction")
    {
        result = cftod(vector<uint64_t>{2, 1, 1, 1, 4}, 10);

        REQUIRE(result == Approx(sqrt(7)));
    };

    SECTION("sqrt(3) from continued fraction")
    {
        result = cftod(vector<uint64_t>{1, 1, 2}, 10);

        REQUIRE(result == Approx(sqrt(3)));
    };

    SECTION("pi as continued fraction")
    {
        result = cftod(vector<uint64_t>{3, 7, 15, 1, 292, 1});

        REQUIRE(result == Approx(pi));
    };
};