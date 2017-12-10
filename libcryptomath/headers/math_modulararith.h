/*! \file */ 
#pragma once

#include <limits>
#include <stdexcept>
#include <array>
#include <vector>
#include <cmath>
#include <type_traits>

#include "math_misc.h"

#ifndef DBGOUT
#define DBGOUT(a) 
#endif

namespace cryptomath
{

/*! \brief Computes a (mod b), properly handling negative values
 
\param[in] a The top of the rational to convert
\param[in] b The bottom of the rational to convert

\returns Integral - a (mod b)
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

/*! Computes \f$ a^b\f$ mod \f$ n \f$

Fast power mod function taken from Applied Cryptography by Bruce Schneier.
It makes use of the powers of two present in \f$ b \f$ to compute the result.

Template arguments
    - Integral - Some integer type

\param[in] a
\param[in] b
\param[in] n
\returns Integral - \f$ a^b\f$ mod \f$ n \f$
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

/*! \brief PowMod wrapper to prevent overflow

If n-1 is greater than the square root of the maximum value of Integral,
then the _powMod function will overflow the type. This wrapper throws an
exception if that is the case. Specialize this template for any type which
cannot be used with std::numeric_limits.

Template arguments
    - Integral - Some integer type

\param[in] a
\param[in] b
\param[in] n
\returns Integral - \f$ a^b\f$ mod \f$ n \f$
\throws logic_error : Computing the power mod n would overflow type Integral
*/
template <class Integral>
Integral powMod(const Integral& a, const Integral& b, const Integral& n) {
    using std::sqrt;
    if(n-1 > sqrt(std::numeric_limits<Integral>::max()))
        throw std::logic_error("powmod integer overflow");
    return _powMod<Integral>(a, b, n);
}

/*! \brief Recursive gcd calculation that assumes a, b unsigned.

For general use, call ::gcd

Uses the Euclidian gcd formula. If a % b is non-0, then the solution is the same as
gcd(b, a % b)

Template arguments
    - Integral - Some integer type
 
\param[in] a
\param[in] b

\returns Integral - gcd(a, b)
*/
template<class Integral>
Integral _unsignedgcd(const Integral& a, const Integral& b)
{
    //DBGOUT("GCD(" << a << ", " << b << ")")
    Integral modAb = a % b;
    return modAb == 0 ? b : _unsignedgcd<Integral>(b, modAb);
}

/*! \brief Computes gcd(|a|, |b|).

If a or b is 0, the other is returned. gcd(0, 0) returns 0.

Template arguments
    - Integral - Some integer type

\param[in] a
\param[in] b

\returns Integral - gcd(|a|, |b|)
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

/*! \brief Computes the extended gcd of a, b

Finds a solution for a*x + b*y = gcd(|a|, |b|) using the extended Euclidian gcd. 
For each iteration until a % b = 0
    - \f$ q(i) = a/b \f$ 
    - \f$ x(i) = -q(i)*x(i-1) + x(i-2)\f$ 
    - \f$ y(i) = -q(i)*y(i-1) + y(i-2) \f$ 
    - \f$ b, a = a%b, b \f$ 

Template arguments
    - Integral - Some integer type

\param[in] a_
\param[in] b_

\returns array<Integral, 3> - [gcd(|a|, |b|), x, y]
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

/*! \brief Finds \f$ a^{-1} \f$ mod \f$ n \f$

Finds the modular inverse of a (mod n). That is, finds x such that \f$ a*x = 1 \f$ mod \f$ n \f$.
Using the extended gcd of a and n, if a and n are relatively prime, then the solution is x (mod n) from
the equation \f$ ax + ny = 1 \f$

Template arguments
    - Integral - Some integer type

\param[in] a
\param[in] n

\returns Integral - \f$ a^{-1} \f$ mod \f$ n \f$ (0 if non-existant)
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

/*! \brief Computes the Legendre symbol

The Legendre symbol \f$ (\frac{a}{p}) \f$ is defined as \f$ a^{(p-1)/2} \f$ mod \f$ p \f$ for prime values \f$ p \f$

Template arguments
    - Integral - Some integer type

\param[in] a
\param[in] p A prime number
\returns Integral - \f$ (\frac{a}{p}) \f$
\throws logic_error : powmod would overflow the Integral type
*/
template<class Integral>
Integral legendre(const Integral& a, const Integral& p)
{
    return powMod<Integral>(a, (p-1)/2);
}

/*! \brief Computes the jacobi symbol

This function should not be called directly. Call the wrapper jacobi() to prevent type issues

The Jacobi symbol \f$ (\frac{a}{n}) \f$ is a generalization of the Legendre symbol to all odd integers. One way to compute it
is to find the product of the Legendre symbols for all prime factors of \f$ n \f$. This specific implementation uses the
law of Quadratic Reciprocity to calculate the Jacobi without factoring \f$ n \f$. More details on the algorithm can
be found at http://2000clicks.com/mathhelp/NumberTh27JacobiSymbolAlgorithm.aspx

Template arguments
    - Integral - Some signed integer type

\param[in] a
\param[in] n - Some odd integer
\returns Integral - \f$ (\frac{a}{n}) \f$
\throws logic_error : n is negative or even
*/
template<class Integral>
Integral _jacobi(Integral a, Integral n)
{
    using std::swap;

    if(n <= 0 || mod2<Integral>(n) == 0)
        throw std::logic_error("jacobi of a negative or even base is undefined");
    
    Integral j=1;
    if(a < 0)
    {
        a = -a;
        if(mod<Integral>(n, 4) == 3) 
            j = -j;        
    }

    Integral modded;
    while(a != 0)
    {
        while(mod<Integral>(a, 2) == 0)
        {
            //Process factors of 2: Jacobi(2,b) = -1 if b=3,5 (mod 8)
            a = a/2;
            modded = mod<Integral>(n, 8);
            if(modded == 3 || modded == 5)
                j = -j;
        }
        // Quadratic reciprocity: Jacobi(a,b) = -Jacobi(b,a) if a=3,b=3 (mod 4)
        swap(a, n);

        if(mod<Integral>(a, 4) == 3 && mod<Integral>(n, 4) == 3)
            j = -j;

        a = mod<Integral>(a, n);
    }

    if(n == 1) return j;
    return 0;
}

/*! \brief Jacobi symbol wrapper to guarantee that the type is signed

The recursive Jacobi function requires signed values. This wrapper throws an
exception if the template type is strictly unsigned. Specialize this for any type
which cannot be checked with std::is_unsigned

Template arguments
    - Integral - Some integral type

\param[in] a
\param[in] n - Some odd integer
\returns Integral - \f$ (\frac{a}{n}) \f$
\throws logic_error : Integral is an unsigned type
*/
template<class Integral>
Integral jacobi(const Integral& a, const Integral& n)
{
    if(std::is_unsigned<Integral>::value) 
        throw std::logic_error("jacobi calculation requires signed values");
        
    return _jacobi<Integral>(a, n);
}

}