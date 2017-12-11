/*! \file */ 
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
/*! Removes verbose debug outputs from compiled result */
#define DBGOUT(a) 
#endif

namespace cryptomath
{

/*! Contains algorithms for finding \f$ ab=n \f$ for some odd, non-prime \f$ n \f$ */
namespace factoring
{
    /*! \brief Shanks' square forms factorization
        
    Fermat's factorization algorithm requires finding \f$ x^2 - y^2 = n \f$; however, it can be
    faster to look for some \f$ x^2 = y^2 \f$ (mod \f$ n \f$). While finding such a pair \f$ x, y \f$ does 
    not guarantee a factor of \f$ n \f$, it implies that \f$ n \f$ is a factor of \f$ (x-y)(x+y) \f$. Since there's
    a good chance that the factors of \f$ n \f$ are split between \f$ x-y, x+y \f$, then \f$ gcd(n, x-y) \f$ is likely
    to yield a non-trivial factor of \f$ n \f$.

    Shank's square forms algorithm is a method for finding some \f$ x^2, y^2 \f$ which satisfy this. It starts with some
    small multiple of \f$ n \f$ and walks forward until a square is found, and then walks backwards to find the second square.
    At this point, the gcd of the difference and \f$ n \f$ can be checked. If it is not 1 or \f$ n \f$, a nontrivial factor
    has been found; if it is trivial, the algorithm is repeated with some different multiple of \f$ n \f$

    Template arguments
        - class Integral - Some Integer type

    \param[in] n The value to factor
    \returns pair<Integral, Integral> - Two values with a product \f$ n \f$
    */
    template <class Integral>
    std::pair<Integral, Integral> shanks(const Integral& n)
    {
        using std::swap;
        DBGOUT("Shanks factor " << n);

        //Check for perfect square
        auto sqt = intSqrt<Integral>(n);
        if(sqt.first)
        {
            DBGOUT("Perfect square: " << sqt.second)
            return std::make_pair(sqt.second, sqt.second);
        }

        Integral k = 1;
        while(true)
        {
            DBGOUT("k = " << k)
            Integral pi_1 = sqrtfloor<Integral>(k*n), p0 = pi_1, pi=0;
            Integral qi_1 = 1;
            Integral qi = k*n - p0*p0;
            Integral bi = 0;

            Integral i = 0;
            std::pair<bool, Integral> sqrtq;

            //Forward iterations
            while(mod2<Integral>(i+1) == 1 || !(sqrtq = intSqrt(qi)).first)
            {
                DBGOUT(i << " : " << pi << " | " << qi << " | " << bi);
                i++;
                
                bi = (p0 + pi_1)/qi;

                pi = bi*qi - pi_1;

                qi_1 = qi_1 + bi*(pi_1 - pi);
                swap(qi_1, qi);

                pi_1 = pi;
            }

            //Reverse iterations
            i = 0;
            Integral b0 = (p0 - pi_1)/sqrtq.second;
            p0 = pi = b0*sqrtq.second + pi_1;
            qi_1 = sqrtq.second;
            qi =  (k*n - p0*p0)/qi_1;
            do
            {
                DBGOUT(i << " : " << pi << " | " << qi << " | " << bi);
                i++;

                pi_1 = pi;

                bi = (p0 + pi_1)/qi;

                pi = bi*qi-pi_1;

                qi_1 = qi_1+bi*(pi_1-pi);
                swap(qi_1, qi);
            }while(pi != pi_1);

            Integral f = gcd<Integral>(n, pi);
            DBGOUT("f = " << f)
            if(f != 1 && f != n)
                return std::make_pair(f, n/f);

            k++;
        }
    }

    /*! \brief Fermat's factorization algorithm

    Fermat's factorization algorithm is based on the fact that every odd number can be
    written as the difference of two squares. (\f$ n = a^2 - b^2\f$ for some \f$ a, b\f$). When those values
    are found, we can write \f$ n = (a-b)(a+b) \f$; therefore \f$ a-b, a+b \f$ are factors of \f$ n \f$.

    Fermat's algorithm starts with \f$ a = sqrt(n) \f$ and repeatedly checks if some possible \f$ b^2 = a^2 - n \f$ is
    actually squared. If it is, then \f$ a, b \f$ have been found; if not, \f$ a \f$ is incremented.

    Template arguments
        - class Integral - Some Integer type

    \param[in] n The value to factor
    \returns pair<Integral, Integral> - Two values with a product \f$ n \f$
    */
    template <class Integral>
    std::pair<Integral, Integral> fermat(const Integral& n)
    {
        DBGOUT("Fermat factor " << n);
        Integral a = sqrtfloor<Integral>(n);
        Integral b2 = a*a - n;
        
        while(a*a < n || !intSqrt<Integral>(b2).first)
        {
            a = a + 1;
            b2 = a*a - n;
            DBGOUT(a << ", " << b2);
        }

        Integral sqt = sqrtfloor<Integral>(b2);
        return std::make_pair(a + sqt, a - sqt); 
    }

    /*! \f$ a^2 + 1 \f$ mod \f$ n \f$
    A function to be used in Pollard's rho factorization algorithm

    Template arguments
        - class Integral - Some Integer type

    \param[in] a
    \param[in] n
    \returns \f$ a^2 + 1 \f$ mod \f$ n \f$
    */
    template <class Integral>
    Integral pRho1(const Integral& a, const Integral& n)
    {
        return mod<Integral>(a*a + 1, n);
    }

    /*! \f$ a^2 - 1 \f$ mod \f$ n \f$
    A function to be used in Pollard's rho factorization algorithm

    Template arguments
        - class Integral - Some Integer type

    \param[in] a
    \param[in] n 
    \returns \f$ a^2 - 1 \f$ mod \f$ n \f$
    */
    template <class Integral>
    Integral pRho2(const Integral& a, const Integral& n)
    {
        Integral x = a*a;

        //Prevent integer underflow in unsigned types
        if(x == 0) x = n;
        return mod<Integral>(x - 1, n);
    }

    /*! Pollard's Rho algorithm for factoring

    Pollard's Rho algorithm makes use of the cyclical nature of polynomials mod some \f$ n \f$. Given some
    function \f$ g(x) \f$, it can be used to generate a sequence of values \f$ x_0 = x, x_1 = g(x), x_2 = g(g(x))... \f$.
    
    This sequence can be taken mod \f$ p \f$ for some prime to generate a new sequence. Since the original sequence is mod \f$ n \f$
    and the second is mod \f$ p \f$, we know they will eventually start to repeat. We can even say that the second sequence, \f$ x_k \f$ mod \f$ p \f$
    is likely to repeat much sooner because of the birthday paradox.

    By stepping both sequences together, and checking the gcd of \f$ k_i, k_j \f$ we can find a factor of \f$ n \f$. Specifically,
    when the gcd is not 1, we know one of the sequences is repeating because \f$ k_i = k_j \f$ mod \f$ p \f$; therefore
    the difference between them is a multiple of \f$ p \f$ and the gcd is one of the factors of \f$ n \f$.
    
    Pollards Rho algorithm generates the two sequences as 
        - \f$ k_{i+1} = g(k_i) \f$
        - \f$ k_{j+1} = g(g(k_j)) \f$

    If we reach a cycle and the gcd of the two numbers is \f$ n \f$, then a different function is used, or a different initial
    value is tried

    Template arguments
        - class Integral - Some Integer type

    \param[in] n The value to factor
    \returns pair<Integral, Integral> - Two values with a product \f$ n \f$
    */
    template <class Integral>
    std::pair<Integral, Integral> pollardrho(const Integral& n)
    {
        std::vector<std::function<Integral(const Integral&, const Integral&)>> PRhoFuncs {
            pRho1<Integral>,
            pRho2<Integral>};

        DBGOUT("Factor pollardrho " << n);
        for(Integral a_ = 2; ; a_++)
        {
            DBGOUT("a -> " << a_);
            for(auto g : PRhoFuncs)
            {
                Integral b = a_;
                Integral d = 1;
                Integral a = a_;
                while(d == 1)
                {
                    DBGOUT("a " << a);
                    DBGOUT("g(a) " << g(a, n));                    
                    a = g(a, n);
                    b = g(g(b, n), n);
                    d = gcd<Integral>(a-b, n);
                    DBGOUT(a << " " << b << " " << d);
                }
                if(d != 0 && d != n) return std::pair<Integral, Integral>(d, n/d);
            }
        }
    }

    /*! \brief Pollard's p-1 factoring algorithm 

    Pollard's p-1 algorithm leverages Fermat's little theorem and the idea that
    \f$ a^{K(p-1)} = 1 \f$ mod \f$ p \f$ for some prime and all positive \f$ k \f$.

    Because of this, we know that if some \f$ x \f$ is congruent to 1 mod a factor of some \f$ n \f$
    then the gcd of \f$ x-1 \f$ and \f$ n \f$ will be divisible by that factor.

    So, we make \f$ K(p-1) \f$ very large by using prime powers, we can use this test to find factors of \f$ n \f$. We pick some
    \f$ x \f$ and repeatedly do \f$ x = x^\omega \f$ where \f$ \omega \f$ is some prime power. (This ensures that we have \f$ K(p-1) \f$ with lots
    of prime factors). When the gcd of \f$ x-1 \f$ and \f$ n \f$ is not 1 or \f$ n \f$ we have found a factor of \f$ n \f$.

    Template arguments
        - class Integral - Some Integer type

    \param[in] n The value to factor
    \returns pair<Integral, Integral> - Two values with a product \f$ n \f$
    \throws logic_error : powmod would overflow the Integral type
    */
    template <class Integral>
    std::pair<Integral, Integral> pollardp1(const Integral& n)
    {
        DBGOUT("Factor pollardrp1 " << n);        
        Integral s = 2;
        Integral b_initial = 2;
        while(true)
        {
            //Start with b
            Integral b = b_initial;

            //Get powers of b mod n up to some point
            for(Integral j = 2; j < s-1; j++)
            {
                b = powMod<Integral>(b, j, n);
            }

            //Check successive powers for a factor
            for(Integral j = s-1; j < n; j++)
            {
                b = powMod<Integral>(b, j, n);            
                DBGOUT("b = " << b);
                Integral d = gcd<Integral>(b-1, n);
                DBGOUT("d = " << d);
                if(1 < d && d < n) return std::pair<Integral, Integral>(d, n/d);
            }

            //If we get to the end, try a larger b to start
            do
            {
                b_initial++;
            }while(gcd<Integral>(b_initial, n) != 1);
        }
    }
}

//! Enum containing all factoring methods available
enum class Factor_Method{Fermat, PollardRho, Shanks, PollardP_1};

/*! \brief General factoring algorithm

This function will use one of the specific factoring functions in the factoring namespace
to find all prime factors of a number. It factors out all powers of 2, and checks for primality; 
while factors are not prime, they are factored using the specified algorithm.

Template arguments
    - class Integral - Some Integer type

\param[in] n The value to factor
\param[in] m The method of factorization to use (Default Pollard's Rho algorithm)
\returns vector<Integral, Integral> - All prime factors of \f$ n \f$ sorted from smallest to largest. 
    If \f$ n \f$ is prime, it contains only \f$ n \f$. 
    Factors which are used multiple times are included multiple times
*/
template <class Integral>
std::vector<Integral> factor(const Integral& n, const Factor_Method& m = Factor_Method::PollardRho)
{
    DBGOUT("Factor " << n << " method " << (int)m);
    const static std::map<Factor_Method, std::function<std::pair<Integral, Integral>(const Integral&)>> algos
    {
        {Factor_Method::Fermat, factoring::fermat<Integral>},
        {Factor_Method::PollardRho, factoring::pollardrho<Integral>},
        {Factor_Method::PollardP_1, factoring::pollardp1<Integral>},
        {Factor_Method::Shanks, factoring::shanks<Integral>}
    };

    //Check if the number is 1 or 0
    if(n == 1 || n == 0) return std::vector<Integral>{n};

    auto algo = algos.at(m);

    //Queue up n
    std::queue<Integral> composites;
    composites.push(n);

    std::vector<Integral> out;

    //While there are possibly composite factors
    while(composites.size())
    {
        Integral i = composites.front();
        DBGOUT("Check factor " << i);

        //Check the factor for primality
        if(isPrime<Integral>(i))
        {
            //If so, add to output
            DBGOUT("Is prime!");
            out.push_back(i);
        }
        //Factor out all twos and add them
        //to factorization
        else if(mod2<Integral>(i) == 0)
        {
            DBGOUT("Factoring 2's");
            auto p = factor2s<Integral>(i);
            for(int j=0; j<p.first; j++)
                out.push_back(2);
            composites.push(p.second);
        }
        //Factor into two parts and add them to the list
        else if(i > 1)
        {
            std::pair<Integral, Integral> factors = algo(i);
            composites.push(factors.first);
            composites.push(factors.second);
            DBGOUT("Factored to " << factors.first << " " << factors.second);
        }
        composites.pop();
    }

    std::sort(out.begin(), out.end());
    return out;
}

/*! Calculates \f$ \phi(n) \f$

This function calculates Euler's totient function for any value using
only integer math.

Template arguments
    - class Integral - Some integer type
\param[in] n The number to find the totient of
\returns Integral - \f$ \phi(n) \f$
*/
template<class Integral>
Integral phi(Integral n)
{    
    DBGOUT("Phi(" << n << ")");
    Integral result = n; 
 
    std::vector<Integral> factors = factor(n, Factor_Method::PollardRho);

    for(const Integral& q : factors)
    {
        DBGOUT("Factor " << q);
        if(n % q == 0)
        {
            do
            {
                n = n / q;
            }while( n % q == 0);
            result = result - result / q;
        }
    }

    if(n > 1)
    {
        DBGOUT("Non-0 n");
        result = result - result / n;
    }

    return result;
}

/*! \brief Checks if some value is a primitive root

A primitive root \f$ x \f$ mod \f$ n \f$ is any value for which all values coprime to \f$ n \f$ can be found
as some \f$ x^k \f$ mod \f$ n \f$ for some \f$ k \f$. If \f$ n \f$ is prime, then all primitive roots of it are
generators of the set \f$ Z_n \f$. For any primitive root, its order (number of successive powers before repeat)
is \f$ \phi(n) \f$

We can check if some \f$ a \f$ is a primitive root mod \f$ n \f$ with a couple of rules
    - For some prime \f$ p \f$, a primitive root will satisfy \f$ a^{\phi(p)/q_i} \neq 1 \f$ mod \f$ p \f$ for all factors \f$ q_i \f$ of \f$ \phi(p) \f$
    - Other than 1, 2, and 4, numbers with primitive roots are of the form \f$ p^k \f$ or \f$ 2p^k \f$ for some odd prime \f$ p \f$
    - The order of a primitive root mod \f$ n \f$ is \f$ \phi(n) \f$
    - If \f$ a \f$ is a primitive root of \f$ p \f$, then either \f$ a \f$ or \f$ a + p \f$ is a primitive root of all \f$ p^k \f$
    - If some odd \f$ a \f$ is a primitive root of \f$ p \f$, then \f$ a \f$ is a primitive root of all \f$ 2p^k \f$

So, to check if \f$ a \f$ is a primitive root mod \f$ n \f$, we first check that \f$ n \f$ is of the form of a number with primitive roots. Then
we factor \f$ n \f$ to get it's prime factor. We check if \f$ a \f$ is a primitive root of that prime factor. If that prime factor is the only factor
then \f$ n \f$ is prime and we are done. Otherwise, we check if the order of \f$ a \f$ is \f$ \phi(n) \f$ mod \f$ p^2 \f$. If 2 is not a factor
of \f$ n \f$, then we are done; otherwise, we check if \f$ a \f$ is odd.

Template arguments
    - class Integral - Some integer value

\param[in] a The number to test if it's a primitive root
\param[in] n The number to test \f$ a \f$ modded by
\returns bool - Whether or not \f$ a \f$ is a primitive root mod \f$ n \f$
\throws logic_error : powmod would overflow the Integral type
*/
template<class Integral>
bool isPrimitiveRoot(Integral a, const Integral& n)
{
    DBGOUT(a << " is primitive root mod " << n << "?");

    //Trivial cases
    if(n <= 1) return false;
    
    //Get a in the range [0, n)
    a = mod<Integral> (a, n);
    if(n <= 4) return a == n-1;

    DBGOUT(" -> " << a << " mod " << n);

    //Factor n
    std::vector<Integral> factors = factor<Integral>(n, Factor_Method::PollardRho);

    bool pk2 = false;

    //Guaranteed to be at least one factor
    //Check if n might be of the form 2p^k
    if(factors[0] == 2)
    {
        DBGOUT("Maybe 2p^k");
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
    DBGOUT("p = " << p << "?")
    for(const Integral& i : factors)
    {
        if(i != p) return false;
    }

    //0 is never a primitive root
    if(mod(a, p) == 0) return false;    

    //Check if a is a primitive root mod the prime factor p of n

    //Factor p-1
    Integral p1 = p-1;
    std::vector<Integral> p1factors = factor<Integral>(p1, Factor_Method::PollardRho);
    DBGOUT("Factored " << p1);
    for(auto q = p1factors.begin(); q != p1factors.end(); q++)
    {
        //Check each unique factor once
        if(q == p1factors.begin() || *q != *(q-1))
        {
            DBGOUT("Testing " << a << " ^ ( " << p1 << " / " << *q << " )");
            //if a^((p-1)/q) mod p is 1, then it's not a primitive root
            if(powMod<Integral>(a, p1 / *q, p) == 1) return false;
        }
    }

    DBGOUT(a << "is primitive root of " << p);

    //We know a is a primitive root of the prime factor p of n. If this is the only factor of
    //n (n is prime) we are done
    if(factors.size() == 1 && !pk2) return true;

    //Check if a is a root of p^2, if so it is a root of all p^k
    Integral p2 = p*p;
    Integral phin = phi(p2);

    DBGOUT("Testing for order " << phin);    
    for(Integral i=1; i<phin; i++)
    {
        if(powMod<Integral>(a, i, p2) == 1) return false;
    }

    bool rootpk = powMod<Integral>(a, phin, p2) == 1;
    DBGOUT(a << (rootpk ? " is" : " is not") << " a primitive root of " << p << " ^ k");
    
    if(rootpk)
    {
        //If n is the form 2pk, then if r a primitive root of p^k is odd, it's a primitive root;
        //else r + p^k is a primitive root (but not r)
        if(pk2)
        {
            DBGOUT("2p^k, testing even or odd");
            return mod2<Integral>(a) == 1;
        }
        else
        {
            return true;
        }
    }

    return false;
}

}