#ifndef CRYPTO_MATH_H
#define CRYPTO_MATH_H

#include <array>
#include <cmath>

#ifndef DEBUG
    #define DBGOUT(a) 
#else
    #include <iostream>
    #define DBGOUT(a) std::cerr << a << std::endl;
#endif

namespace cryptomath
{

//Returns a (mod b)
//Since c++ % operator doesn't quite handle negative numbers
//as needed for working in Z_p
template<class Integral>
Integral mod(const Integral& a, const Integral& b)
{
    DBGOUT(a << " (mod " << b << ")")
    if(b < 0) return -mod<Integral>(-a, -b);
    if(a < 0)
    {
        Integral t = mod<Integral>(-a, b);
        return t == 0 ? t : b - t;
    }

    return a % b;
}

//Utility function; call gcd(a, b) in regular use
template<class Integral>
Integral _unsignedgcd(const Integral& a, const Integral& b)
{
    DBGOUT("GCD(" << a << ", " << b << ")")
    Integral modAb = a % b;
    return modAb == 0 ? b : _unsignedgcd<Integral>(b, modAb);
}

template <class Integral>
Integral gcd(Integral a, Integral b)
{
    DBGOUT("GCD(" << a << ", " << b << ")")    
    //If a or b is negative, make them positive
    //and run unsigned gcd
    if(a < 0) a = -a;
    if(b < 0) b = -b;
    if(b == 0) return a;
    if(a == 0) return b;

    return _unsignedgcd<Integral>(a, b);
}

template <class Integral>
std::array<Integral, 3> extendedGcd(Integral a_, Integral b_)
{
    DBGOUT("Ext GCD(" << a_ << ", " << b_ << ")")
    
    if(a_ < 0) a_ = -a_;
    if(b_ < 0) b_ = -b_;

    //Swap a and b if b is greater
    Integral a = a_ < b_ ? b_ : a_;
    Integral b = a_ < b_ ? a_ : b_;

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

    DBGOUT(b << " " << xi << " " << yi)    
    return std::array<Integral, 3> {b, xi, yi};
}

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

#endif