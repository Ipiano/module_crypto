/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

using namespace std;
using namespace cryptomath;

/*!
    \test Tests edge cases of the GCD.
        - GCD(0, n)
        - GCD(n, 0)
        - GCD(0, 0)
*/
TEST_CASE("Gcd edge cases")
{
    SECTION("GCD(0, 10)")
    {
        unsigned int soln = gcd(0, 10);
        REQUIRE(soln == 10);
    }

    SECTION("GCD(10, 0)")
    {
        unsigned int soln = gcd(10, 0);
        REQUIRE(soln == 10);
    }

    SECTION("GCD(0, 0)")
    {
        unsigned int soln = gcd(0, 0);
        REQUIRE(soln == 0);
    }
}
/*!
    \test Tests a case with the GCD both with and without GMP
        - 484, 1180
        - 1180, 484
*/
TEST_CASE("The gcd function")
{
    SECTION("GCD(482, 1180)")
    {
        unsigned int soln = gcd(482, 1180);
        REQUIRE(soln == 2);
    }

    SECTION("GCD(1180, 482)")
    {
        unsigned int soln = gcd(1180, 482);
        REQUIRE(soln == 2);
    }

#ifdef CRYPTOMATH_GMP    
    SECTION("GMP Support: GCD(482, 1180)")
    {
        mpz_class a(1180), b(482);
        mpz_class soln = gcd<mpz_class>(a, b);
        REQUIRE(soln == 2);
    }

    SECTION("GMP Support: GCD(1180, 482)")
    {
        mpz_class a(1180), b(482);
        mpz_class soln = gcd<mpz_class>(b, a);
        REQUIRE(soln == 2);
    }
#endif
}