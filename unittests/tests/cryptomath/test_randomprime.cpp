/*! @file */ 
#include "../../catch.hpp"

#include "bbs.h"
#include "cryptomath.h"

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

using namespace std;
using namespace cryptomath;

TEST_CASE("The randomPrime function")
{
    typedef bbs::blum_blum_shub_engine<uint8_t, uint64_t> generator;
    generator reng(1300843, 1300927);

    SECTION("Smallish")
    {
        for(int i=0; i<10; i++)
        {
            constexpr uint64_t b = 10;
            uint64_t p = randomPrime<uint64_t, generator>(reng, b);
            REQUIRE(isPrime<uint64_t>(p));
            REQUIRE(p > powInt<uint64_t>(2, b));
            REQUIRE(p < powInt<uint64_t>(2, b+1));
        }

        for(int i=0; i<10; i++)
        {
            constexpr uint64_t b = 12;
            uint64_t p = randomPrime<uint64_t, generator>(reng, b);
            REQUIRE(isPrime<uint64_t>(p));
            REQUIRE(p > powInt<uint64_t>(2, b));
            REQUIRE(p < powInt<uint64_t>(2, b+1));
        }

        for(int i=0; i<10; i++)
        {
            constexpr uint64_t b = 14;
            uint64_t p = randomPrime<uint64_t, generator>(reng, b);
            REQUIRE(isPrime<uint64_t>(p));
            REQUIRE(p > powInt<uint64_t>(2, b));
            REQUIRE(p < powInt<uint64_t>(2, b+1));
        }
    };

    SECTION("Bigish")
    {
        for(int i=0; i<10; i++)
        {
            constexpr uint64_t b = 20;
            uint64_t p = randomPrime<uint64_t, generator>(reng, b);
            REQUIRE(isPrime<uint64_t>(p));
            REQUIRE(p > powInt<uint64_t>(2, b));
            REQUIRE(p < powInt<uint64_t>(2, b+1));
        }
    };

#ifdef CRYPTOMATH_GMP
    SECTION("GMP compatible")
    {
        for(int i=0; i<10; i++)
        {
            constexpr uint64_t b = 100;
            mpz_class p = randomPrime<mpz_class, generator>(reng, b);
            REQUIRE(isPrime<mpz_class>(p)) ;
            REQUIRE(p > powInt<mpz_class>(2, b));
            REQUIRE(p < powInt<mpz_class>(2, b+1));
        }
    };
#endif
}