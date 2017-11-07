/*! @file */ 
#pragma once

#include <array>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <queue>
#include <random>
#include <limits>
#include <functional>
#include <cmath>

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

    //DBGOUT(a << " (mod " << b << ") = " << a % b);
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
        DBGOUT("Fermat factor " << n);
        Integral a = sqrtfloor<Integral>(n);
        Integral b2 = a*a - n;
        
        while(a*a < n || powInt<Integral>(sqrtfloor<Integral>(b2), 2) != b2)
        {
            a = a + 1;
            b2 = a*a - n;
            DBGOUT(a << ", " << b2);
        }

        return std::make_pair(a + sqrtfloor<Integral>(b2), a - sqrtfloor<Integral>(b2)); 
    }

    template <class Integral>
    Integral pRho1(const Integral& a)
    {
        return a*a + 1;
    }

    template <class Integral>
    Integral pRho2(const Integral& a)
    {
        return a*a - 1;
    }

    template <class Integral>
    std::pair<Integral, Integral> pollardrho(const Integral& n)
    {
        std::vector<std::function<Integral(const Integral&)>> PRhoFuncs {
            pRho1<Integral>,
            pRho2<Integral>};

        DBGOUT("Factor pollardrho " << n);
        for(Integral a_ = 2; ; a_++)
        {
            DBGOUT("a -> " << a_);
            for(auto g : PRhoFuncs)
            {
                Integral b = 2;
                Integral d = 1;
                Integral a = a_;
                while(d == 1)
                {
                    DBGOUT("a " << a);
                    DBGOUT("g(a) " << g(a));                    
                    a = g(a);
                    b = g(g(b));
                    d = gcd<Integral>(a-b, n);
                    DBGOUT(a << " " << b << " " << d);
                }
                if(d != n) return std::pair<Integral, Integral>(d, n/d);
            }
        }
    }

    template <class Integral>
    std::pair<Integral, Integral> pollardp1(const Integral& n)
    {
        DBGOUT("Factor pollardrp1 " << n);        
        Integral s = 2;
        Integral b_initial = 2;
        while(true)
        {
            Integral b = b_initial;
            for(Integral j = 2; j < s-1; j++)
            {
                b = powMod<Integral>(b, j, n);
            }
            for(Integral j = s-1; j < n; j++)
            {
                b = powMod<Integral>(b, j, n);            
                DBGOUT("b = " << b);
                Integral d = gcd<Integral>(b-1, n);
                DBGOUT("d = " << d);
                if(1 < d && d < n) return std::pair<Integral, Integral>(d, n/d);
            }
            do
            {
                b_initial++;
            }while(gcd(b_initial, n) != 1);
        }
    }
}

enum class Factor_Method{Fermat, PollardRho, PollardP_1};
template <class Integral>
std::vector<Integral> factor(const Integral& n, const Factor_Method& m)
{
    const static std::map<Factor_Method, std::function<std::pair<Integral, Integral>(const Integral&)>> algos
    {
        {Factor_Method::Fermat, factoring::fermat<Integral>},
        {Factor_Method::PollardRho, factoring::pollardrho<Integral>},
        {Factor_Method::PollardP_1, factoring::pollardp1<Integral>}
    };

    if(n == 1 || n == 0) return std::vector<Integral>{n};

    auto algo = algos.at(m);
    std::queue<Integral> composites;
    composites.push(n);

    std::vector<Integral> out;

    while(composites.size())
    {
        Integral i = composites.front();
        if(isPrime<Integral>(i))
        {
            out.push_back(i);
        }
        else if(i % 2 == 0)
        {
            auto p = factor2s<Integral>(i);
            for(int j=0; j<p.first; j++)
                out.push_back(2);
            composites.push(p.second);
        }
        else if(i > 1)
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

/* Source https://math.stackexchange.com/questions/1416422/how-does-one-find-the-primitive-roots-of-a-non-prime-number*/
template<class Integral>
bool isPrimitiveRoot(Integral a, const Integral& n)
{
    //Get a in the range [0, n)
    a = mod<Integral> (a, n);

    //Trivial cases
    if(n == 1) return true;
    if(n == 2) return false;
    if(n == 4) return a == 2;

    //Factor n
    std::vector<Integral> factors = factor<Integral>(n, Factor_Method::PollardRho);

    bool pk2 = false;

    //Guaranteed to be at least one factor
    //Check if n might be of the form 2p^k
    if(factors[0] == 2)
    {
        pk2 = true;
        //If 2 is a factor, there's guaranteed to be at least some other factor
        //because n is not 2
        factors.erase(factors.begin());
    }

    //Check that all factors are the same
    //If so, then n is prime, n is p^k or n is 2p^k
    //If n is not this form, then there are no primitive roots
    //mod n
    Integral p = factors[0];
    for(const Integral& i : factors)
    {
        if(i != p) return false;
    }

    //Check if a is a primitive root mod the prime factor p of n

    //Factor p-1
    Integral p1 = p-1;
    std::vector<Integral> p1factors = factor<Integral>(p1, Factor_Method::PollardRho);
    for(auto q = p1factors.begin(); q != p1factors.end(); p1factors++)
    {
        //Check each unique factor once
        if(q == p1factors.begin() || *q != *(q-1))
        {
            //if a^((p-1)/q) mod p is 1, then it's not a primitive root
            if(powMod<Integral>(a, p1 / *q, p) == 1) return false;
        }
    }

    //We know a is a primitive root of the prime factor p of n. If this is the only factor of
    //n (n is prime) we are done
    if(factors.size() == 1 && !pk2) return true;

    //If n is the form 2pk, then if r odd, it's a primitive root; else r + p^k is a primitive root (but not r)
    if(pk2)
        return a % 2 == 1;

    //Check if a is a root of p^2, if so it is a root of all p^k
    Integral p2 = p*p;
    for(Integral i=2; i<p2-1; i++)
    {
        if(powMod<Integral>(a, i, p2) == 1) return false;
    }
    return powMod<Integral>(a, p2-1, p2) == 1;
}

}