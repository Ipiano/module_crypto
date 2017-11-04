/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#include <gmpxx.h>

using namespace std;
using namespace cryptomath;

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

    SECTION("5 (mod -3)")
    {
        int soln = mod(5, -3);
        REQUIRE(soln == -1);
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

    SECTION("GMP Support: 5 (mod 3)")
    {
        mpz_class a(5), b(3);
        mpz_class soln = mod<mpz_class>(a, b);
        REQUIRE(soln == 2);
    }
}