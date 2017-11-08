/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

using namespace std;
using namespace cryptomath;

TEST_CASE("The inverseMod function")
{
    SECTION("18^-1 (mod 8)")
    {
        int soln = inverseMod(18, 8);
        REQUIRE(soln == 0);
    }

    SECTION("4^-1 (mod 13)")
    {
        int soln = inverseMod(4, 13);
        REQUIRE(soln == 10);
    }
    
#ifdef CRYPTOMATH_GMP        
    SECTION("GMP Support: 3^-1 (mod 2)")
    {
        mpz_class a(3), b(2);
        mpz_class soln = inverseMod<mpz_class>(a, b);
        REQUIRE(soln == 1);
    }

    SECTION("GMP Support: 18000^-1 (mod 533000389)")
    {
        mpz_class a(18000), b("533000389");
        mpz_class soln = inverseMod<mpz_class>(a, b);
        REQUIRE(soln == mpz_class("458706057"));
    }
#endif
}