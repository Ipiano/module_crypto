#pragma once

#include <limits>
#include <stdexcept>
#include <array>
#include <vector>
#include <cmath>

#include "math_misc.h"

#ifndef DBGOUT
#define DBGOUT(a) 
#endif

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

namespace cryptomath
{

/*! @brief Computes a (mod b), properly handling negative values
 *  
 * @param[in] a The top of the rational to convert
 * @param[in] b The bottom of the rational to convert
 *
 * @returns Integral - a (mod b)
 */
template<class Integral>
Integral mod(const Integral& a, const Integral& b)
{
    if(b < 0) return -mod<Integral>(-a, -b);
    if(a < 0)
    {
        Integral t = mod<Integral>(-a, b);
        return t == 0 ? t : b - t;
    }

    //DBGOUT(a << " (mod " << b << ") = " << a % b);
    return a % b;
}

/*! Computes a^b mod n
 *
 * Taken from Applied Cryptography by Bruce Schneier (Linked from
 * Wikipedia)
 */
template<class Integral>
Integral _powMod(Integral a, Integral b, const Integral& n) {
    if(n == 1) return 0;

    DBGOUT("powmod(" << a << ", " << b << ", " << n << ")");
    a = mod<Integral>(a, n);
    Integral result = 1;

    while (b > 0)
    {
        if (mod2<Integral>(b) == 1)
            result = mod<Integral>((result * a), n);
        a = mod<Integral>((a * a), n);
        b = b / 2;
    }
    DBGOUT(result)
    return result;
}

template <class Integral>
Integral powMod(const Integral& a, const Integral& b, const Integral& n) {
    if(n-1 > std::sqrt(std::numeric_limits<Integral>::max()))
        throw std::logic_error("powmod integer overflow");
    return _powMod<Integral>(a, b, n);
}

#ifdef CRYPTOMATH_GMP
template<>
mpz_class inline powMod<mpz_class>(const mpz_class& a, const mpz_class& b, const mpz_class& n) {
    return _powMod<mpz_class>(a, b, n);
}
#endif


/*! @brief Finds a^-1 (mod n)
 *  
 * Finds the modular inverse of a (mod n). That is, finds x such that a*x = 1 (mod n).
 * Using the extended gcd of a and n, if a and n are relatively prime, then the solution is x (mod n) from
 * the equation ax + ny = 1
 *
 * @param[in] a
 * @param[in] n
 *
 * @returns Integral - a^-1 (mod n) (0 if non-existant)
 */
template <class Integral>
Integral inverseMod(const Integral& a, const Integral& n)
{
    DBGOUT("inverseMod(" << a << ", " << n << ")")
    std::array<Integral, 3> sln = extendedGcd<Integral>(a, n);
    if(sln[0] == 1)
    {
        return mod<Integral>(sln[1], n);
    }
    return 0;
}

}