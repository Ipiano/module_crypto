/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#include <array>
#include <iostream>
#include <set>

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

using namespace std;
using namespace cryptomath;

/*!
    \test Tests the primitive root test function and that it works with GMP
        - Roots of 0, 1
        - Roots of 2, 3, 4
        - Roots of 7
        - Roots of 62
        - Roots of 31
        - Roots of 49
        - Roots of 50
        - Roots of 63
        - Roots of 27
*/
TEST_CASE("The isPrimitiveRoots function")
{
    SECTION("Primitive roots mod 0-1")
    {
        for(int n=0; n<2; n++)
            for(int i=0; i<100; i++)
            {
                REQUIRE(isPrimitiveRoot(i, n) == false);
            }
    }

    SECTION("Primitive roots mod 2-4")
    {
        for(int n=2; n<5; n++)
            for(int i=0; i<n; i++)
            {
                REQUIRE(isPrimitiveRoot(i, n) == (i == n-1));
            }
    }

    SECTION("Primitive roots mod 7")
    {
        set<uint64_t> roots{3, 5};
        uint64_t n = 7;

        for(uint64_t i=0; i<n; i++)
        {
            REQUIRE(isPrimitiveRoot(i, n) == roots.count(i));
        }
    }

    SECTION("Primitive roots mod 62")
    {
        set<uint64_t> roots{3, 11, 13, 17, 21, 43, 53, 55};
        uint64_t n = 62;

        for(uint64_t i=0; i<n; i++)
        {
            REQUIRE(isPrimitiveRoot(i, n) == roots.count(i));
        }
    }

    SECTION("Primitive roots mod 31")
    {
        set<uint64_t> roots{3, 11, 12, 13, 17, 21, 22, 24};
        uint64_t n = 31;

        for(uint64_t i=0; i<n; i++)
        {
            REQUIRE(isPrimitiveRoot(i, n) == roots.count(i));
        }
    }

    SECTION("Primitive roots mod 49")
    {
        set<uint64_t> roots{3, 5, 10, 12, 17, 24, 26, 33, 38, 40, 45, 47};
        uint64_t n = 49;

        for(uint64_t i=0; i<n; i++)
        {
            REQUIRE(isPrimitiveRoot(i, n) == roots.count(i));
        }
    }

    SECTION("Primitive roots mod 50")
    {
        set<uint64_t> roots{3, 13, 17, 23, 27, 33, 37, 47};
        uint64_t n = 50;

        for(uint64_t i=0; i<n; i++)
        {
            REQUIRE(isPrimitiveRoot(i, n) == roots.count(i));
        }
    }

    SECTION("Primitive roots mod 63")
    {
        set<uint64_t> roots;
        uint64_t n = 63;

        for(uint64_t i=0; i<n; i++)
        {
            REQUIRE(isPrimitiveRoot(i, n) == roots.count(i));
        }
    }

#ifdef CRYPTOMATH_GMP        
    SECTION("GMP Support: Primitive roots mod 27")
    {
        set<mpz_class> roots{2, 5, 11, 14, 20, 23};
        mpz_class n(27);

        for(mpz_class i(0); i<n; i++)
        {
            REQUIRE(isPrimitiveRoot<mpz_class>(i, n) == roots.count(i));
        }
    }
#endif
}