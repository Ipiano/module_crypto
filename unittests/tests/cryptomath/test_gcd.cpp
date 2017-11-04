/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#include <gmpxx.h>

using namespace std;
using namespace cryptomath;

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
}