/*! \file */ 
#pragma once

#include <cmath>
#include <array>

#ifndef DBGOUT
/*! Removes verbose debug outputs from compiled result */
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
    - class Integral - Some integer type

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
    - class Integral - Some integer type

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
    - class Integral - Some integer type

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
    - class Integral - Some integer type

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
    - class Integral - Some integer type

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
    - class Integral - Some integer type

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


/*! \brief Checks if it is possible that a number is square

It can be show that any perfect square must be of one of the following forms: 00, e1, e4, 25, o6, e9 where e is any
even digit and o is any odd digit. Using this rule, we can rule out some numbers as perfect squares quickly.

Proof:

Any number can be written in the form \f$ 100a + 10b + c \f$ where \f$ b, c \f$ are the last two digits of the number
and \f$ a \f$ is the rest of the digits. If we square this, we obtain
\f[10000a^2 + 2000ab + 200ac + 100b^2 + 20bc + c^2 \f]
The first four terms are guaranteed to be greater than 100, so we care about \f$20bc + c^2 \f$. Notice that \f$ 20bc \f$ is guaranteed
to contribute an even amount to the 10's place. If we write out all possible values for \f$ c^2 \f$, we can see that 
all the of listed forms except 00 and 25 are accounted for. The only case to end with 0 is when \f$ c=0 \f$, so therefore
\f$ 20bc = 0\f$, which means 00 is a valid form. The only way to end with 5 is when \f$ c = 5 \f$. In this case, \f$ 20bc \f$
becomes \f$ 100b \f$ and therefore does not contribute to the last two digits; so we are left with \f$ c^2 = 25 \f$ asthe last
two digits

Template arguments
    - class Integral - Some integer type

\param[in] n The number to check for possible squareness
\returns bool - Whether or not it's possible that n is square
*/
template<class Integral>
bool isMaybeSquare(const Integral& n)
{
    Integral digs2 = n % 100;
    if(digs2 == 0 || digs2 == 25) return true;

    Integral tens = digs2/10;
    Integral ones = digs2 % 10;
    if(mod2<Integral>(tens) == 0)
    {
        if(ones == 1 || ones == 4 || ones == 9)
            return true;
    }
    else if(ones == 6) return true;

    return false;
}

/*! \brief Returns the square root of a square integer as an integer

Utilizes the isMaybeSquare function to quickly rule out some numbers as being square.
If the number might be square, it is checked by computing the square of the floor of its square root.

Template arguments
    - class Integral - Some integer type

\param[in] n The number to square root
\returns pair<bool, Integral> - [false, 0] if n not square, [true, sqrt(n)] if it is
*/
template<class Integral>
std::pair<bool, Integral> intSqrt(const Integral& n)
{
    if(n == 0) return std::pair<bool, Integral>(true, 0);
    if(!isMaybeSquare<Integral>(n)) return std::pair<bool, Integral>(false, 0);

    Integral candidate = sqrtfloor<Integral>(n);
    if(candidate*candidate == n) return std::pair<bool, Integral>(true, candidate);

    return std::pair<bool, Integral>(false, 0);
}


}