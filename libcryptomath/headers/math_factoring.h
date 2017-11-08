#pragma once

#include <utility>
#include <vector>
#include <functional>
#include <queue>
#include <algorithm>
#include <map>

#include "./math_misc.h"
#include "./math_primality.h"
#include "./math_modulararith.h"

#ifndef DBGOUT
#define DBGOUT(a) 
#endif

namespace cryptomath
{

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
            }while(gcd<Integral>(b_initial, n) != 1);
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
        else if(mod2<Integral>(i) == 0)
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
        return mod2<Integral>(a) == 1;

    //Check if a is a root of p^2, if so it is a root of all p^k
    Integral p2 = p*p;
    for(Integral i=2; i<p2-1; i++)
    {
        if(powMod<Integral>(a, i, p2) == 1) return false;
    }
    return powMod<Integral>(a, p2-1, p2) == 1;
}


}