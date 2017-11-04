/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#include <gmpxx.h>
#include <utility>

using namespace std;
using namespace cryptomath;

TEST_CASE("The factor2s function")
{
    SECTION("Powers of 2")
    {
        REQUIRE(factor2s(0) == make_pair(0, 0));
        REQUIRE(factor2s(1) == make_pair(0, 1));        
        REQUIRE(factor2s(2) == make_pair(1, 1));
        REQUIRE(factor2s(4) == make_pair(2, 1));
        REQUIRE(factor2s(8) == make_pair(3, 1));
        REQUIRE(factor2s(262144) == make_pair(18, 1));
        REQUIRE(factor2s(33554432) == make_pair(25, 1));
    };

    SECTION("Odd numbers")
    {
        REQUIRE(factor2s(3) == make_pair(0, 3));
        REQUIRE(factor2s(5) == make_pair(0, 5));
        REQUIRE(factor2s(387) == make_pair(0, 387));
        REQUIRE(factor2s(3298457) == make_pair(0, 3298457));
        REQUIRE(factor2s(32343) == make_pair(0, 32343));
    };

    SECTION("Nontrivial numbers")
    {
        REQUIRE(factor2s(56) == make_pair(3, 7));
        REQUIRE(factor2s(636416) == make_pair(9, 1243));
        REQUIRE(factor2s(186336) == make_pair(5, 5823));
        REQUIRE(factor2s(1580928) == make_pair(7, 12351));
        REQUIRE(factor2s(15708) == make_pair(2, 3927));        
    }

    SECTION("GMP compatible")
    {
        REQUIRE(factor2s(mpz_class(636416)) == make_pair(mpz_class(9), mpz_class(1243)));
        REQUIRE(factor2s(mpz_class(33554432)) == make_pair(mpz_class(25), mpz_class(1)));     
        REQUIRE(factor2s(mpz_class(32343)) == make_pair(mpz_class(0), mpz_class(32343)));        
    };
}