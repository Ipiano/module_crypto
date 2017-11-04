/*! @file */ 
#pragma once

#include <array>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <random>
#include <limits>

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

#ifndef DEBUG
    /*! Removes verbose debug outputs from compiled result */
    #define DBGOUT(a) 
#else
    #include <iostream>
    /*! Enables verbose debug outputs from compiled result */
    #define DBGOUT(a) std::cerr << a << std::endl;
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

    DBGOUT(a << " (mod " << b << ") = " << a % b);
    return a % b;
}

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
    DBGOUT("GCD(" << a << ", " << b << ")")
    Integral modAb = a % b;
    return modAb == 0 ? b : _unsignedgcd<Integral>(b, modAb);
}

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
    using std::abs;

    DBGOUT("GCD(" << a << ", " << b << ")")    
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
    using std::abs;
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
        DBGOUT("b = " << b);
        DBGOUT("a = " << a);
        DBGOUT("r = " << result);

        if (b % 2 == 1)
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
#endif

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
    while((d % 2) == 0)
    {
        DBGOUT("Fac: " << fac);
        DBGOUT("LG2: " << lg2);
        while(d % fac == 0)
        {
            DBGOUT("D: " << d);
            DBGOUT("R: " << r);

            d = d / fac;
            r = r + lg2;
        }
        fac = fac / 2;
        lg2 = lg2 - 1;
    }

    DBGOUT("D: " << d);
    DBGOUT("R: " << r);
    
    return std::make_pair(r, d);
}

/*! Implementation of Miller-Rabin primality test
 *
 * The chance it returns a false positive is (1/4)^iterations
 */
template<class Integral>
bool isPrime(const Integral& n, const uint64_t& iterations = 10)
{
    if(n == 2) return true;
    if(n < 3 || n%2 == 0) return false;

    std::mt19937_64 reng;
    std::uniform_int_distribution<uint64_t> dist(2);

    std::pair<Integral, Integral> rd = factor2s<Integral>(n-1);
    Integral& r = rd.first;
    Integral& d = rd.second;

    for(int i=0; i<iterations; i++)
    {
        uint64_t a_ = dist(reng);
        Integral a = mod<Integral>(Integral(a_), n-4) + 2;
        DBGOUT("a_ = " << a_ << " a = " << a);

        Integral x = powMod<Integral>(a, d, n);
        bool n1;       
        DBGOUT("x0 = " << x); 
        if(x != 1 && x != n-1)
        {
            n1 = false;
            for(int j=1; j<r; j++)
            {
                DBGOUT("j: " << j << " x = " << x);
                x = powMod<Integral>(x, 2, n);
                if(x == 1)
                {
                    DBGOUT("x == 1");
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
                DBGOUT("!n1");
                return false;
            }
        }
    }
    return true;
}

template<class Integral>
Integral sqrtfloor(const Integral& n)
{
    return std::sqrt(n);
}

#ifdef CRYPTOMATH_GMP
template<>
mpz_class inline sqrtfloor<mpz_class>(const mpz_class& n)
{
    mpz_class out;
    mpz_sqrt(out.get_mpz_t(), n.get_mpz_t());
    return out;
}
#endif

/*! Contains factoring algorithms */
namespace factoring
{
    template <class Integral>
    std::pair<Integral, Integral> fermat(const Integral& n)
    {
        Integral a = sqrtfloor(n);
        Integral b2 = a*a - n;

        while(b2 < 0 || powInt(sqrtfloor(b2), 2) != b2)
        {
            a++;
            b2 = a*a - n;
        }

        return std::make_pair(a + sqrt(b2), a - sqrt(b2)); 
    }

    template <class Integral>
    std::pair<Integral, Integral> pollardrho(const Integral& n)
    {
        
    }

    template <class Integral>
    std::pair<Integral, Integral> pollardp1(const Integral& n)
    {
        
    }
}

enum class Factor_Method{Fermat, PollardRho, PollardP_1};
template <class Integral>
std::vector<Integral> factor(const Integral& n, const Factor_Method& m)
{
    const static std::unordered_map<Factor_Method, std::function<std::pair<Integral, Integral>(const Integral&)>> algos =
    {
        {Factor_Method::Fermat, factoring::fermat},
        {Factor_Method::PollardRho, factoring::pollardrho},
        {Factor_Method::PollardP_1, factoring::pollardp1}
    };

    auto algo = algos.at(m);
    std::queue<Integral> composites { n };
    std::vector<Integral> out;

    while(composites)
    {
        Integral i = composites.front();
        if(isPrime(i))
        {
            out.push_back(i);
        }
        else
        {
            std::pair<Integral, Integral> factors = algo(i);
            composites.push(factors.first);
            composites.push(factors.second);
        }
        composites.pop();
    }

    std::sort(out.begin(), out.end());
    return out;
}

}