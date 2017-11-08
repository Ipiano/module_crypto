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
        REQUIRE(isPrime(2) == true);        
        REQUIRE(isPrime(3) == true);
        REQUIRE(isPrime(5) == true);
        REQUIRE(isPrime(7) == true);
        REQUIRE(isPrime(11) == true);
        REQUIRE(isPrime(13) == true);
        REQUIRE(isPrime(15761) == true);
        REQUIRE(isPrime(27179) == true);
        REQUIRE(isPrime(38699) == true);
        REQUIRE(isPrime((uint64_t)57719) == true);
    };

    SECTION("Non prime numbers")
    {
        REQUIRE(isPrime(0) == false);
        REQUIRE(isPrime(1) == false);
        REQUIRE(isPrime(4) == false);
        REQUIRE(isPrime(8) == false);
        REQUIRE(isPrime(21) == false);
        REQUIRE(isPrime((uint64_t)96083) == false);
        REQUIRE(isPrime((uint64_t)87699) == false);
        REQUIRE(isPrime(44175) == false);
        REQUIRE(isPrime((uint64_t)57725) == false);
    };

#ifdef CRYPTOMATH_GMP    
    SECTION("GMP compatible")
    {
        mpz_class a(57725);
        REQUIRE(isPrime<mpz_class>(a) == false);

        a = mpz_class(57719);
        REQUIRE(isPrime<mpz_class>(a) == true);        
    };
#endif
}