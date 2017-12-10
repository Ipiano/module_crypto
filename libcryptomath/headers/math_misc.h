/*! \file */ 
#pragma once

#include <cmath>
#include <array>

#ifndef DBGOUT
#define DBGOUT(a) 
#endif

namespace cryptomath
{

/*! \brief Checks if a value is odd

Standard templated call for checking if a value is odd. This exists
because it is often faster to test the least significant bit than to call a modulus
operator, and the bitwise operators are not standard for GMP. For any integer type
that doesn't implement bitwise operators, this can be specialized to provide an efficient
odd test.

Template arguments
    - Integral - Some integer type

\param[in] n Value to test for odd-ness
\returns uint8_t - n % 2
*/
template<class Integral>
uint8_t mod2(const Integral& n)
{
    return (uint8_t)(n & 1);
}

/*! \brief Floor of square root

Template arguments
    - Integral - Some integer type

\param[in] n Value to square root
\returns Integral - Square root of n rounded down
*/
template<class Integral>
Integral sqrtfloor(const Integral& n)
{
    return std::sqrt(n);
}

/*! \brief Integer pow function

Template arguments
    - Integral - Some integer type

\param[in] a Base
\param[in] b Power to take a to
\returns Integral - a^b
*/
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

/*! \brief Log base 2

Template arguments
    - Integral - Some integer type

\param[in] n Value to square root
\returns Integral -Square root of n rounded down
*/
template<class Integral>
uint64_t log2(const Integral& n)
{
    return std::log2(n);
}

/*! \brief Tests if a type has a certain number of bits

Can be specialized for arbitrary precision types to always
return true

Template arguments
    - Integral - Some integer type

\param[in] i Number of bits
\returns bool - Whether or not Integral type has at least i bits
*/
template<class Integral>
bool hasBits(const uint64_t& i)
{
    return sizeof(Integral)*8 >= i;
}

/*! \brief Absolute value

Templated absolute value function which can be specialized
for non standard integer types

Template arguments
    - Integral - Some integer type

\param[in] a Value to absolute
\returns Integral - |a|
*/
template<class Integral>
Integral abs(const Integral& a){ 
    using std::abs;
    return abs(a);
}

/*! \brief Absolute value

Specialization of abs for unsigned 64 bit int so that
other functions which need an absolute value can work with
unsigned values

\param[in] a Value to absolute
\returns uint64_t - |a|
*/
template<>
uint64_t inline abs<uint64_t>(const uint64_t& a){ return a; }

/*! \brief Absolute value

Specialization of abs for unsigned 32 bit int so that
other functions which need an absolute value can work with
unsigned values

\param[in] a Value to absolute
\returns uint32_t - |a|
*/
template<>
uint32_t inline abs<uint32_t>(const uint32_t& a){ return a; }

/*! \brief Absolute value

Specialization of abs for unsigned 8 bit int so that
other functions which need an absolute value can work with
unsigned values

\param[in] a Value to absolute
\returns uint8_t - |a|
*/
template<>
uint8_t inline abs<uint8_t>(const uint8_t& a){ return a; }

}