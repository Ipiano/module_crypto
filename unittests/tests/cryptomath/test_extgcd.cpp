#include "../../catch.hpp"

#include "cryptomath.h"

#include <array>

#include <gmpxx.h>

using namespace std;

TEST_CASE("The extended gcd function")
{
    SECTION("Extended GCD(482, 1180)")
    {
        array<int, 3> soln = extendedGcd(482, 1180);
        REQUIRE(soln[0] == 2);
        REQUIRE(soln[1] == 71);
        REQUIRE(soln[2] == -29);
    }

    SECTION("Extended GCD(1180, 482)")
    {
        array<int, 3> soln = extendedGcd(1180, 482);
        REQUIRE(soln[0] == 2);
        REQUIRE(soln[1] == 71);
        REQUIRE(soln[2] == -29);
    }

    SECTION("GMP Support: Extended GCD(482, 1180)")
    {
        mpz_class a(482), b(1180);
        array<mpz_class, 3> soln = extendedGcd(a, b);
        REQUIRE(soln[0] == 2);
        REQUIRE(soln[1] == 71);
        REQUIRE(soln[2] == -29);
    }

    SECTION("GMP Support: Extended GCD(1180, 482)")
    {
        mpz_class a(482), b(1180);        
        array<mpz_class, 3> soln = extendedGcd(b, a);
        REQUIRE(soln[0] == 2);
        REQUIRE(soln[1] == 71);
        REQUIRE(soln[2] == -29);
    }
}