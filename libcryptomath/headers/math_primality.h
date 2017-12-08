/*! @file */ 
#pragma once

#include <random>
#include <utility>
#include <map>
#include <functional>

#include "./math_misc.h"
#include "./math_modulararith.h"

#ifndef DBGOUT
    #define DBGOUT(a) 
#endif

namespace cryptomath
{

/*! Factors n into 2^r * d
 *
 *
 */
template<class Integral>
std::pair<Integral, Integral> factor2s(const Integral& n)
{
    DBGOUT("Factor 2's " << n);
    if(n == 0) return std::make_pair(Integral(0), Integral(0));
    
    Integral lg2(log2<Integral>(n));
    Integral fac = powInt<Integral>(2, lg2);
    
    Integral d = n;
    Integral r = 0;
    while(mod2<Integral>(d) == 0)
    {
        while(d % fac == 0)
        {
            d = d / fac;
            r = r + lg2;
        }
        fac = fac / 2;
        lg2 = lg2 - 1;
    }
    
    DBGOUT(r << ", " << d);
    return std::make_pair(r, d);
}

template<class Integral>
void sundaramSieve(const Integral& n, std::vector<Integral>& result)
{
    result.clear();
    if(n > 2) result.push_back(2);

    Integral markCount = (n-2)/2;
    std::vector<bool> marks(markCount+1, false);

    for(Integral i = 1; i <= markCount; i++)
        for(Integral j = i; (i + j + 2*i*j) <= markCount; j++)
            marks[i + j + 2*i*j] = true;
    
    for(Integral i = 1; i <= markCount; i++)
        if(!marks[i]) 
            result.push_back(2*i + 1);
}

namespace primality
{
    /*! Implementation of Miller-Rabin primality test
    *
    * The chance it returns a false positive is (1/4)^iterations
    */
    template<class Integral>
    bool millerRabin(const Integral& n, const uint64_t& iterations = 10)
    {
        DBGOUT("MillerRabin(" << n << ")");
        std::mt19937_64 reng;
        std::uniform_int_distribution<uint64_t> dist(2);

        std::pair<Integral, Integral> rd = factor2s<Integral>(n-1);
        Integral& r = rd.first;
        Integral& d = rd.second;

        for(int i=0; i<iterations; i++)
        {
            uint64_t a_ = dist(reng);

            Integral a = mod<Integral>(Integral(a_), n-4) + 2;
            Integral x = powMod<Integral>(a, d, n);
            bool n1;       
            if(x != 1 && x != n-1)
            {
                n1 = false;
                for(int j=1; j<r; j++)
                {
                    x = powMod<Integral>(x, 2, n);
                    if(x == 1)
                    {
                        return false;
                    }
                    if(x == n-1)
                    {
                        n1 = true;
                        break;
                    }
                }
                if(!n1)
                {
                    return false;
                }
            }
        }
        return true;
    }

    template<class Integral>
    bool solovayStrassen(const Integral& n, const uint64_t& iterations=10)
    {
        DBGOUT("SolovayStrassen(" << n << ")");
        
        std::mt19937_64 reng;
        std::uniform_int_distribution<uint64_t> dist(2);

        Integral n12 = (n-1)/2;

        for(uint64_t i = 0; i < iterations; i++)
        {
            Integral a_ = dist(reng);
            Integral a = mod<Integral>(Integral(a_), n-4) + 2;

            DBGOUT("Jacobi(" << a << ", " << n << ") = " << jacobi<Integral>(a, n));
            
            if(mod<Integral>(jacobi<Integral>(a, n), n) != powMod<Integral>(a, n12, n))
                return false;
        }
        return true;
    }
}

enum class Primality_Test{MillerRabin, SolovayStrassen};

template<class Integral>
bool isPrime(const Integral& n, const Primality_Test& test = Primality_Test::MillerRabin, const uint64_t& iterations = 20)
{
    const static std::map<Primality_Test, std::function<bool(const Integral&, const uint64_t&)>> algos
    {
        {Primality_Test::MillerRabin, primality::millerRabin<Integral>},
        {Primality_Test::SolovayStrassen, primality::solovayStrassen<Integral>}        
    };

    if(n == 2 || n == 3) return true;
    if(n < 3 || mod2<Integral>(n) == 0) return false;

    return algos.at(test)(n, iterations);
}

template<class Integral>
Integral nextPrime(Integral start, const uint64_t& reps = 20)
{
    if(start < 2) return 2;

    //Get next odd number
    if(mod2<Integral>(start) == 0) start++;
    else start += 2;

    //Check all odd numbers > start for next prime
    while(!isPrime(start, Primality_Test::MillerRabin, reps)) start += 2;
    
    return start;
}

template<class Integral, class UniformRandomBitGenerator>
Integral _randomPrime(UniformRandomBitGenerator& bits, const uint64_t& bitcount, const uint64_t& prime_reps = 20)
{
    DBGOUT("Random prime: " << bitcount);
    Integral result = 1;
    uint8_t bit;

    //Pick a bunch of random bits
    for(int i=0; i < bitcount; i++)
    {
        bit = mod2<typename UniformRandomBitGenerator::result_type>(bits());
        result = result*2 + bit;
    }

    //Find next prime, and min/max range
    result = nextPrime<Integral>(result, prime_reps);
    Integral max = powInt<Integral>(2, bitcount+1);
    Integral min = powInt<Integral>(2, bitcount);

    //If result too big, cut in half to get back in range
    if(result > max)
        result = result / 2;

    //Get new primes while too small
    do
    {
        result = nextPrime<Integral>(result, prime_reps);
    }while(result < min);

    return result;
}

template<class Integral, class UniformRandomBitGenerator>
Integral randomPrime(UniformRandomBitGenerator& bits, const uint64_t& bitcount, const uint64_t& prime_reps = 20)
{
    //Ensure that 2^(bitcount+1) fits in the type
    if(!hasBits<Integral>(bitcount+2))
        throw std::logic_error("type not large enough for random prime with specific length");

    return _randomPrime<Integral, UniformRandomBitGenerator>(bits, bitcount, prime_reps);
}

}