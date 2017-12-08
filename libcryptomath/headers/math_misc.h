/*! @file */ 
#pragma once

#include <cmath>
#include <array>

#ifndef DBGOUT
#define DBGOUT(a) 
#endif

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

namespace cryptomath
{

template<class Integral>
uint8_t mod2(const Integral& n)
{
    return (uint8_t)(n & 1);
}

template<class Integral>
Integral sqrtfloor(const Integral& n)
{
    return std::sqrt(n);
}

template<class Integral>
Integral powInt(const Integral& a, const Integral& b)
{
    if(b == 0) return 1;
    if(b == 1) return a;

    Integral n = powInt<Integral>(a, b/2);
    if(mod2<Integral>(b) == 1)
        return n * n * a;
    return n * n;
}

template<class Integral>
uint64_t log2(const Integral& n)
{
    return std::log2(n);
}

template<class Integral>
bool hasBits(const uint64_t& i)
{
    return sizeof(Integral)*8 >= i;
}
    
#ifdef CRYPTOMATH_GMP
template<>
bool inline hasBits<mpz_class>(const uint64_t& i)
{
    return true;
}

template<>
uint64_t inline log2<mpz_class>(const mpz_class& n)
{
    return mpz_sizeinbase(n.get_mpz_t(), 2);
}

template<>
mpz_class inline sqrtfloor<mpz_class>(const mpz_class& n)
{
    mpz_class out;
    mpz_sqrt(out.get_mpz_t(), n.get_mpz_t());
    return out;
}

template<>
uint8_t inline mod2<mpz_class>(const mpz_class& n)
{
    return (uint8_t)(mpz_tstbit(n.get_mpz_t(), 0));
}

#endif

/*! @brief Recursive gcd calculation that assumes a, b unsigned.
 *
 * For general use, call ::gcd
 *
 * Uses the Euclidian gcd formula. If a % b is non-0, then the solution is the same as
 * gcd(b, a % b)
 *  
 * @param[in] a
 * @param[in] b
 *
 * @returns Integral - gcd(a, b)
 */
template<class Integral>
Integral _unsignedgcd(const Integral& a, const Integral& b)
{
    //DBGOUT("GCD(" << a << ", " << b << ")")
    Integral modAb = a % b;
    return modAb == 0 ? b : _unsignedgcd<Integral>(b, modAb);
}

template<class Integral>
Integral abs(const Integral& a){ 
    using std::abs;
    return abs(a);
}


template<>
uint64_t inline abs<uint64_t>(const uint64_t& a){ return a; }

template<>
uint32_t inline abs<uint32_t>(const uint32_t& a){ return a; }

template<>
uint8_t inline abs<uint8_t>(const uint8_t& a){ return a; }

}