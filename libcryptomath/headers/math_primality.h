/*! \file */ 
#pragma once

#include <random>
#include <utility>
#include <map>
#include <functional>
#include <chrono>

#include "./math_misc.h"
#include "./math_modulararith.h"

#ifndef DBGOUT
/*! Removes verbose debug outputs from compiled result */
    #define DBGOUT(a) 
#endif

namespace cryptomath
{

/*! \brief Factors all powers of 2 out of a number

Many factoring and primality tests only work with odd integers. This
function divides all powers of two out of a number

Template arguments
    - class Integral - Some integer type

\param[in] n - The number to factor 2's from 
\returns pair<Integral, Integral> - [number of 2's factored out, n with all powers of 2 factored out]
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

/*! \brief Sundaram's sieve for finding prime numbers

The sieve of Sundaram finds all the prime numbers less than some \f$ n \f$.
Start with a list of all numbers less than \f$ n \f$.
For all \f$ i, j \f$ greater than equal to 1, where \f$ i+j+2ij\f$ is less than or equal to \f$ n \f$, \f$ i+j+2ij \f$ is removed.
For all remaining values, \f$ i \f$, \f$ 2i + 1 \f$ is prime.

Template arguments
    - class Integral - Some integer type which can be used to index into a vector (mpz_class will not work)

\param[in] n - Number to find primes less than
\param[out] result - Vector to store results in
*/
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

//! Contains specific primality tests
namespace primality
{
    /*! \brief Implementation of Miller-Rabin primality test
    
    Given a number \f$ n \f$, we can determine if it is prime by looking
    at all \f$ a^{d}, a^{2^rd} \f$ mod \f$ n \f$ for some \f$ a \f$ and \f$ r < s-1 \f$ where
    \f$ d, s \f$ are found by factoring \f$ n-1 \f$ into \f$ 2^sd\f$. By Fermat's Little theorem,
    either \f$ a^{d} = 1\f$ or \f$ a^{2^rd} = -1\f$. 

    The Miller Rabin test uses the contrapositive of this statement. If some \f$ r \f$ can be found
    such that \f$ a^{d} \neq 1\f$ or \f$ a^{2^rd} \neq -1\f$, we know \f$ n \f$ is not prime. If no such \f$ r \f$ 
    can be found, then \f$ a \f$ is a witness of \f$ n \f$ being prime. However, 1/4 of all integers in \f$ Z_n \f$ will
    be a witness even if \f$ n \f$ is not prime, so multiple values should be tested.

    The chance this test returns a false positive is (1/4)^iterations

    Template arguments
        - class Integral - Some integer type

    \param[in] n - The number to test for primality
    \param[in] iterations - The number of witness values to test (Default 10)
    \return bool - Whether or not \f$ n \f$ is probably prime
    */
    template<class Integral>
    bool millerRabin(const Integral& n, const uint64_t& iterations = 10)
    {
        DBGOUT("MillerRabin(" << n << ")");
        auto t = std::chrono::system_clock::now();
        std::mt19937_64 reng(std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count());
        std::uniform_int_distribution<uint64_t> dist(2);

        std::pair<Integral, Integral> rd = factor2s<Integral>(n-1);
        Integral& r = rd.first;
        Integral& d = rd.second;

        //Do a bunch of tests
        for(int i=0; i<iterations; i++)
        {
            //Get a random number
            uint64_t a_ = dist(reng);

            //Make sure it is between 2 and n-2
            Integral a = mod<Integral>(Integral(a_), n-4) + 2;

            //Compute a^d
            Integral x = powMod<Integral>(a, d, n);
            bool n1;   
            
            // Make sure it's not 1, -1
            if(x != 1 && x != n-1)
            {
                n1 = false;
                //For all r < s check if x^(2^rd) is 1 or -1
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

    /*! \brief Implementation of the Solovay-Strassen primality test

    Euler's legendre symbol can be calculated as \f$ (\frac{a}{p}) = a^{(p-1)/2} \f$ for all \f$ p \f$ prime.
    The Jacobi symbol  \f$ (\frac{a}{n}) \f$ is a generalization to all odd integers. One method of defining the Jacobi is in terms
    of the legendre of all prime factors of \f$ n \f$; however, it can also be computed by the Law of Quadratic Reciprocity.

    So, if some \f$ n \f$ is prime, then it stands to reason that the Jacobi should equal the Legendre \f$ (\frac{a}{n}) = a^{(p-1)/2} \f$ for
    all \f$ a \f$.

    Unfortunately, it is not enough to test this for just one \f$ a \f$. There is a 1/2 chance that it will be true for any \f$ a \f$ in \f$ Z_n \f$.
    So, by testing multiple random \f$ a \f$, we can determine that \f$ n \f$ is very likely to be prime.

    The chance this test returns a false positive is (1/2)^iterations

    Template arguments
        - class Integral - Some integer type

    \param[in] n - The number to test for primality
    \param[in] iterations - The number of witness values to test (Default 10)
    \return bool - Whether or not \f$ n \f$ is probably prime
    */
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

//! Enum for available primality tests
enum class Primality_Test{MillerRabin, SolovayStrassen};

/*! General prime test

This prime test function uses one of the prime tests in the primality namespace to check if a number is prime. A couple of trivial cases
are checked for first; anything less than 3 or even is hard-coded.

Template arguments
    - class Integral - Some integer type

\param[in] n The number to test for primality
\param[in] test Which primality test to use
\param[in] iterations Number of iterations to do the probabalistic test
\return bool - Whether or not \f$ n \f$ is probably prime
*/
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

/*! \brief Finds the first prime greater than some number

The next prime is found by testing sequential odd numbers for
primality until one is found to be prime using the Miller-Rabin test

Template arguments
    Integral - Some integer type

\param[in] start - The number to start at
\param[in] reps - Number of iterations to do the probabalistic prime test
\returns Integral - The first prime greater than the starting value
*/
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

/*! \brief Generates a random prime with a specific number of bits

This function should not be called directly; use randomPrime() in order to guaranteed
that the output value will fit in the templated type

It is often useful to have a random prime of a specific number of bits. This function generates
a random number with the specified number of bits, and then finds the first prime larger than that.
If that prime has too many bits, it is divided in half and the first prime greater than that is used.
The Miller-Rabin primality test is used to find the first bigger prime.

Template arguments
    - class Integral - Some integer type
    - class UniformRandomBitGenerator - Some type meeting the C++ UniformRandomBitGenerator specification

\param[in, out] bits - A random bit generator
\param[in] bitcount - Number of bits in final value
\param[in] prime_reps - Number of iterations to do probabalistic primality test
\returns Integral - A random prime with the specified number of bits
*/
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

/*! \brief Wrapper for _randomPrime that verifies that the output type can hold the prime without overflow

For non-standard types, specialize the hasBits templated function to verify that a type can be used

Template arguments
    - class Integral - Some integer type
    - class UniformRandomBitGenerator - Some type meeting the C++ UniformRandomBitGenerator specification

\param[in, out] bits - A random bit generator
\param[in] bitcount - Number of bits in final value
\param[in] prime_reps - Number of iterations to do probabalistic primality test
\returns Integral - A random prime with the specified number of bits
\throws logic_error : Integral type cannot hold a prime of the size requested
*/
template<class Integral, class UniformRandomBitGenerator>
Integral randomPrime(UniformRandomBitGenerator& bits, const uint64_t& bitcount, const uint64_t& prime_reps = 20)
{
    //Ensure that 2^(bitcount+1) fits in the type
    if(!hasBits<Integral>(bitcount+2))
        throw std::logic_error("type not large enough for random prime with specific length");

    return _randomPrime<Integral, UniformRandomBitGenerator>(bits, bitcount, prime_reps);
}

}