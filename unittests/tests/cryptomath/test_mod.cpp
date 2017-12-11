/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

using namespace std;
using namespace cryptomath;

/*!
    Tests the modulus function and that it can be used with GMP
        - 5 mod 3
        - -5 mod 3
        - 5 mod -3
        - -5 mod 3
        - -10 mod 3
        - 10 mod 3
        - 15 mod 3
        - -15 mod 3
        - 3 mod 3
        - -3 mod 3
        - 0 mod 3
*/
TEST_CASE("The mod function")
{
    SECTION("5 (mod 3)")
    {
        int soln = mod(5, 3);
        REQUIRE(soln == 2);
    }

    SECTION("-5 (mod 3)")
    {
        int soln = mod(-5, 3);
        REQUIRE(soln == 1);
    }

    SECTION("5 (mod -3)")
    {
        int soln = mod(5, -3);
        REQUIRE(soln == -1);
    }

    SECTION("-5 (mod -3)")
    {
        int soln = mod(-5, -3);
        REQUIRE(soln == -2);
    }

    SECTION("10 (mod 3)")
    {
        int soln = mod(10, 3);
        REQUIRE(soln == 1);
    }

    SECTION("-10 (mod 3)")
    {
        int soln = mod(-10, 3);
        REQUIRE(soln == 2);
    }

    SECTION("15 (mod 3)")
    {
        int soln = mod(15, 3);
        REQUIRE(soln == 0);
    }

    SECTION("-15 (mod 3)")
    {
        int soln = mod(-15, 3);
        REQUIRE(soln == 0);
    }

    SECTION("3 (mod 3)")
    {
        int soln = mod(3, 3);
        REQUIRE(soln == 0);
    }

    SECTION("-3 (mod 3)")
    {
        int soln = mod(-3, 3);
        REQUIRE(soln == 0);
    }


    SECTION("0 (mod 3)")
    {
        int soln = mod(0, 3);
        REQUIRE(soln == 0);
    }

#ifdef CRYPTOMATH_GMP
    SECTION("GMP Support: 5 (mod 3)")
    {
        mpz_class a(5), b(3);
        mpz_class soln = mod<mpz_class>(a, b);
        REQUIRE(soln == 2);
    }
#endif
}