/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#include <array>
#include <iostream>

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

using namespace std;
using namespace cryptomath;

TEST_CASE("The extended gcd function")
{
    SECTION("Extended GCD(482, 1180)")
    {
        int a = 482;
        int b = 1180;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        cout << soln[1] << " " << soln[1] * a << " " << soln[2] << " " << soln[2] * b;
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(1180, 482)")
    {
        int b = 482;
        int a = 1180;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(-482, 1180)")
    {
        int a = -482;
        int b = 1180;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(482, -1180)")
    {
        int a = 482;
        int b = -1180;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(-1180, 482)")
    {
        int b = 482;
        int a = -1180;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(1180, -482)")
    {
        int b = -482;
        int a = 1180;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(-482, -1180)")
    {
        int a = -482;
        int b = -1180;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(-1180, -482)")
    {
        int b = -482;
        int a = -1180;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(62, 17)")
    {
        int a = 62;
        int b = 17;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(-4, 0)")
    {
        int a = -4;
        int b = 0;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(4, 0)")
    {
        int a = 4;
        int b = 0;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(0, 4)")
    {
        int b = 4;
        int a = 0;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(0, -4)")
    {
        int b = -4;
        int a = 0;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

    SECTION("Extended GCD(0, 0)")
    {
        int b = 0;
        int a = 0;

        array<int, 3> soln = extendedGcd(a, b);
        int ans = gcd(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }

#ifdef CRYPTOMATH_GMP    
    SECTION("GMP Support: Extended GCD(482, 1180)")
    {
        mpz_class a = 482;
        mpz_class b = 1180;

        array<mpz_class, 3> soln = extendedGcd<mpz_class>(a, b);
        mpz_class ans = gcd<mpz_class>(a, b);

        REQUIRE(soln[0] == ans);
        REQUIRE(soln[1]*a + soln[2]*b == soln[0]);
    }
#endif
}