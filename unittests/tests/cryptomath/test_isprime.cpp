/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

using namespace std;
using namespace cryptomath;

TEST_CASE("The isPrime function")
{
    SECTION("Prime numbers")
    {
        for(Primality_Test m = Primality_Test::MillerRabin; m <= Primality_Test::SolovayStrassen; m = (Primality_Test)((int)m+1))
        {
            REQUIRE(isPrime(2, m) == true);        
            REQUIRE(isPrime(3, m) == true);
            REQUIRE(isPrime(5, m) == true);
            REQUIRE(isPrime(7, m) == true);
            REQUIRE(isPrime(11, m) == true);
            REQUIRE(isPrime(13, m) == true);
            REQUIRE(isPrime(15761, m) == true);
            REQUIRE(isPrime(27179, m) == true);
            REQUIRE(isPrime(38699, m) == true);
            REQUIRE(isPrime((int64_t)57719, m) == true);
        }
    };

    SECTION("Non prime numbers")
    {
        for(Primality_Test m = Primality_Test::MillerRabin; m <= Primality_Test::SolovayStrassen; m = (Primality_Test)((int)m+1))
        {
            REQUIRE(isPrime(0, m) == false);
            REQUIRE(isPrime(1, m) == false);
            REQUIRE(isPrime(4, m) == false);
            REQUIRE(isPrime(8, m) == false);
            REQUIRE(isPrime(21, m) == false);
            REQUIRE(isPrime((int64_t)96083, m) == false);
            REQUIRE(isPrime((int64_t)87699, m) == false);
            REQUIRE(isPrime(44175, m) == false);
            REQUIRE(isPrime((int64_t)57725, m) == false);
        }
    };

#ifdef CRYPTOMATH_GMP    
    SECTION("GMP compatible")
    {
        for(Primality_Test m = Primality_Test::MillerRabin; m <= Primality_Test::SolovayStrassen; m = (Primality_Test)((int)m+1))
        {
            mpz_class a(57725);
            REQUIRE(isPrime<mpz_class>(a, m) == false);

            a = mpz_class(57719);
            REQUIRE(isPrime<mpz_class>(a, m) == true); 
        }       
    };
#endif
}