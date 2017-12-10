/*! file */
#pragma once
#include <gmpxx.h>

namespace cryptomath
{

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

template<>
mpz_class inline powMod<mpz_class>(const mpz_class& a, const mpz_class& b, const mpz_class& n) {
    return _powMod<mpz_class>(a, b, n);
}

template<>
mpz_class inline jacobi<mpz_class>(const mpz_class& a, const mpz_class& n) {
    return _jacobi<mpz_class>(a, n);
}

}