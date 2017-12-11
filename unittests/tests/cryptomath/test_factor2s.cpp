/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

#include <utility>

using namespace std;
using namespace cryptomath;

/*!
    \test Tests that powers of 2 can be factored out of values
        - Powers of 2
            - 0
            - 1
            - 2
            - 4
            - 8
            - \f$ 2^18 \f$
            - \f$ 2^25 \f$
        - Odd numbers
            - 3
            - 5
            - 387
            - 3298457
            - 32343
        - Even non-powers of 2
            - 56
            - 636416
            - 186336
            - 1580928
            - 15708
        
    Tests that the following numbers can have 2's factored out
    with mpz_class type
        - 636416
        - 33554432
        - 32343
*/
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

#ifdef CRYPTOMATH_GMP    
    SECTION("GMP compatible")
    {
        REQUIRE(factor2s(mpz_class(636416)) == make_pair(mpz_class(9), mpz_class(1243)));
        REQUIRE(factor2s(mpz_class(33554432)) == make_pair(mpz_class(25), mpz_class(1)));     
        REQUIRE(factor2s(mpz_class(32343)) == make_pair(mpz_class(0), mpz_class(32343)));        
    };
#endif
}