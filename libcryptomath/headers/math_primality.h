#pragma once

#include <random>
#include <utility>

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
    
/*! Implementation of Miller-Rabin primality test
 *
 * The chance it returns a false positive is (1/4)^iterations
 */
template<class Integral>
bool isPrime(const Integral& n, const uint64_t& iterations = 10)
{
    DBGOUT("isprime(" << n << ")");
    if(n == 2 || n == 3) return true;
    if(n < 3 || mod2<Integral>(n) == 0) return false;

    std::mt19937_64 reng;
    std::uniform_int_distribution<uint8_t> dist(2);

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
Integral nextPrime(Integral start)
{
    if(start < 2) return 2;

    //Get next odd number
    if(mod2<Integral>(start) == 0) start++;
    else start += 2;

    //Check all odd numbers > start for next prime
    while(!isPrime(start)) start += 2;
    
    return start;
}

}