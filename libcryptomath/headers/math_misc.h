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
    if(b % 2 == 1)
        return n * n * a;
    return n * n;
}

template<class Integral>
uint64_t log2(const Integral& n)
{
    return std::log2(n);
}
    
#ifdef CRYPTOMATH_GMP
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

/*! @brief Computes gcd(|a|, |b|).
 *
 * If a or b is 0, the other is returned. gcd(0, 0) returns 0.
 *  
 * @param[in] a
 * @param[in] b
 *
 * @returns Integral - gcd(|a|, |b|)
 */
template <class Integral>
Integral gcd(Integral a, Integral b)
{
    //DBGOUT("GCD(" << a << ", " << b << ")")    
    //If a or b is negative, make them positive
    //and run unsigned gcd
    a = abs(a);
    b = abs(b);
    if(b == 0) return a;
    if(a == 0) return b;

    return _unsignedgcd<Integral>(a, b);
}

/*! @brief Computes the extended gcd of a, b
 *  
 * Finds a solution for a*x + b*y = gcd(|a|, |b|) using the extended Euclidian gcd. 
 *
 *  For each iteration until a % b = 0
 *      - q(i) = a/b; 
 *      - x(i) = -q(i)*x(i-1) + x(i-2)
 *      - y(i) = -q(i)*y(i-1) + y(i-2) 
 *      - b, a = a%b, b
 *
 * @param[in] a_
 * @param[in] b_
 *
 * @returns array<Integral, 3> - [gcd(|a|, |b|), x, y]
 */
template <class Integral>
std::array<Integral, 3> extendedGcd(Integral a_, Integral b_)
{
    using std::swap;

    DBGOUT("Ext GCD(" << a_ << ", " << b_ << ")")
    
    bool a_neg = a_ < 0;
    bool b_neg = b_ < 0;

    if(a_ == 0 && b_ == 0)
        return std::array<Integral, 3>{0, 0, 0};    
    if(a_ == 0)
        return std::array<Integral, 3>{abs(b_), 0, abs(b_)/b_};
    if(b_ == 0)
        return std::array<Integral, 3>{abs(a_), abs(a_)/a_, 0};

    a_ = abs(a_);
    b_ = abs(b_);

    bool swapped = false; //a_ < b_;
    //Swap a and b if b is greater
    Integral a = swapped ? b_ : a_;
    Integral b = swapped ? a_ : b_;

    //xi and yi trackers
    Integral x0 = 0, x1 = 1, xi;
    Integral y0 = 1, y1 = 0, yi;

    Integral q, modAb;
    while(a % b != 0)
    {
        modAb = a % b;
        q = a/b;
        xi = -q*x1 + x0;
        yi = -q*y1 + y0;

        DBGOUT(q << " " << xi << " " << yi)

        x0 = x1;
        x1 = xi;

        y0 = y1;
        y1 = yi;

        a = b;
        b = modAb;
    }

    //if(swapped) swap(xi, yi);
    if(a_neg) yi = -yi;
    if(b_neg) xi = -xi;

    DBGOUT(b << " " << yi << " " << xi)    
    return std::array<Integral, 3> {b, yi, xi};
}

}