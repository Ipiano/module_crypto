/*! file */
#pragma once
#include <gmpxx.h>

namespace cryptomath
{

/*! Template specialization of hasBits() for mpz_class

Always returns true because mpz_class type has an unlimited number of bits

\param[in] i - Number of bits needed
\returns bool - true
*/
template<>
bool inline hasBits<mpz_class>(const uint64_t& i)
{
    return true;
}

/*! Template specialization of log2() for mpz_class

Returns log base 2 of the number. No log function exists in the GMP library, so
the number of bits required to represent the number is used instead

\param[in] n - Number to take log2 of
\returns uint64_t - Number of bits needed to represent n
*/
template<>
uint64_t inline log2<mpz_class>(const mpz_class& n)
{
    return mpz_sizeinbase(n.get_mpz_t(), 2);
}

/*! Template specialization of sqrtfloor() for mpz_class

Returns the floor of the square root of a number as an integer

\param[in] n - Number to find square root of
\returns mpz_class - Floor of square root of n
*/
template<>
mpz_class inline sqrtfloor<mpz_class>(const mpz_class& n)
{
    mpz_class out;
    mpz_sqrt(out.get_mpz_t(), n.get_mpz_t());
    return out;
}

/*! Template specialization of mod2() for mpz_class

GMP class types do not support binary operators, but the easiest
way to mod 2 is by checking the LSB. This is done with the special
function GMP provides for that

\param[in] n - The number to mod by 2
\returns uint8_t - n modded by 2
*/
template<>
uint8_t inline mod2<mpz_class>(const mpz_class& n)
{
    return (uint8_t)(mpz_tstbit(n.get_mpz_t(), 0));
}

/*! Template specialization of powMod() for mpz_class

The powMod() function tests that the result type can hold the output
of the calculation without overflowing. This check is removed
for the mpz_class type

\param[in] a
\param[in] b
\param[in] n
\returns Integral - \f$ a^b\f$ mod \f$ n \f$
*/
template<>
mpz_class inline powMod<mpz_class>(const mpz_class& a, const mpz_class& b, const mpz_class& n) {
    return _powMod<mpz_class>(a, b, n);
}

/*! Template specialization of jacobi() for mpz_class

The jacobi() function tests that the result type is not
unsigned. There is no unsigned mpz_class type, so this check
is removed

\param[in] a
\param[in] n - Some odd integer
\returns Integral - \f$ (\frac{a}{n}) \f$
*/
template<>
mpz_class inline jacobi<mpz_class>(const mpz_class& a, const mpz_class& n) {
    return _jacobi<mpz_class>(a, n);
}

}