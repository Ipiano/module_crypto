/*! \file */ 
#pragma once

#include <random>
#include <limits>
#include <cmath>
#include <stdexcept>

#include "cryptomath.h"

/*! Contains the Blum Blum Shub random engine */
namespace bbs
{
    /*! \brief A random engine implementing the Blum Blum Shub pseudorandom generator algorithm which satisfies the C++ UniformRandomBitGenerator concept
    The Blum Blum Shub pseudorandom generation algorithm is as follows
       -# Pick two primes \f$ p, q \f$ such that \f$ p \f$ and \f$ q \f$ are congruent to 3 mod 4
       -# Define \f$ n = p * q \f$
       -# Pick a seed \f$ x \f$ such that \f$ x \f$ is coprime to \f$ n \f$
       -# \f$ x_0 = x^2\f$ (mod \f$ n \f$)
       -# Repeatedly yield \f$ x_n \f$ where \f$ x_n = x_n^2 \f$ (mod \f$ n \f$)

    This algorithm makes use of quadratic-residue to generate random bit sequences. Since \f$ x \f$ is coprime to
    \f$ n \f$, we can fairly confident that successive powers of \f$ x \f$ will yield a sequence of numbers which will not
    repeat for a very long time. By repeatedly doing \f$ x^2 \f$, we turn this into a one-way function because square roots
    mod \f$ n \f$ are difficult to compute. Furthermore, since only the last bits of each number are returned, we can be confident
    that, even if the most significant portion of the number gets repeated, the bits outputted will be pseudo-random.
     
    Notes
        - To maximize the cycle length of the generator, it is recommended that \f$ gcd(\phi(p), \phi(q)) \f$ be small
        - The maximum number of bits that can be safely returned from each \f$ x_n \f$ is \f$ log_2(log_2(n)) \f$

    This generator also satisfies the C++ concept UniformRandomBitGenerator, which means it can be used in
    conjunction with any of the C++ random distributions to yield a sequence of random numbers that from that distribution.

    Template arguments
       - class Result - Unsigned result type to return yielded bits in
       - class Integral - Type to use for internal computation
       - uint64_t bits - Number of least significant bits of each \f$ x_n \f$ to return (default 1)
       - uint32_t prime_reps - Number of repetitions to use in Miller-Rabin prime test function (default 25)
    */
    template<class Result = uint32_t, class Integral = uint64_t, uint64_t bits = 1, uint64_t prime_reps = 25>    
    class blum_blum_shub_engine
    {
        Integral m; /*!< Product of p, q. Values are generated mod m */
        Integral x_prev; /*!< Previous calculated value \f$ x_n \f$ */
    public:

        typedef Result result_type; /*!< Typedef of return type to satisfy UniformRandomBitGenerator */

        /*! Returns minimum possible value output by the generator
         
         Needed to satisy UniformRandomBitGenerator
         *
         @returns The smallest value that can be contained by the templated Result
         */
        constexpr static result_type min() { return std::numeric_limits<result_type>::min(); }

        /*! Returns maximum possible value output by the generator
         
         Needed to satisy UniformRandomBitGenerator
         
         @returns The largest value that can be contained by the templated Result
         */
        constexpr static result_type max() { return std::numeric_limits<result_type>::max(); }

        /*! Constructs a new random generator with \f$ p, q \f$ and optional seed \f$ x \f$
         
        \param p The first prime to use to find \f$ n \f$
        \param q The second prime to use to find \f$ n \f$
        \param x Optional initial seed. If not specified, the valid number greater than \f$ p \f$ which is a prime plus 1 is used
         
        \throws domain_error : \f$ p \f$ or \f$ q \f$ is not prime
        \throws domain_error : \f$ p \f$ or \f$ q \f$ is not congruent to 3 mod 4
        \throws domain_error : The generator cannot securely yield the templated number of bits given \f$ p \f$ and \f$ q \f$
        \throws domain_error : \f$ x \f$ is specified and it is not relatively prime to \f$ n \f$
        */
        blum_blum_shub_engine(Integral p, Integral q, Integral x=-1)
        {
            static_assert(sizeof(result_type)*8 >= bits, "Can't fit output bits it output type");

            p = cryptomath::abs(p);
            q = cryptomath::abs(q);

            //Check that p and q are (probably) prime
            if(! cryptomath::isPrime(p, cryptomath::Primality_Test::MillerRabin, prime_reps) ||
               ! cryptomath::isPrime(q, cryptomath::Primality_Test::MillerRabin, prime_reps))
            {
                throw std::domain_error("p or q is not prime");
            }
            
            //Check that p and q are congruent to 3 (mod 4)
            if(p % 4 != 3 ||
               q % 4 != 3)
            {
                throw std::domain_error("p or q is not congruent to 3(mod 4)");                
            }

            //Set n
            m = p*q;  

            uint64_t maxK = cryptomath::log2<uint64_t>(cryptomath::log2<Integral>(m));

            //Check that we can safely extract bits to fill result_type
            //and be cryptographically secure
            if(bits > maxK)
            {
                std::string err = "cannot securely extract the specified number of bits (" + std::to_string(maxK) + " is the limit)";
                throw std::domain_error(err.c_str());
            }

            //If no seed chosen, use (some prime after p) + 1
            if(x < 0)
            {
                x = p;
                do
                {
                    x = cryptomath::nextPrime(x, prime_reps);
                    x = x+1;
                }while(x <= 1 && cryptomath::gcd(x, m) != 1);
            }
            else
            {
                x = cryptomath::abs(x);

                if(x <= 1 || cryptomath::gcd(x, m) != 1)
                {
                    throw std::domain_error("x is not relatively prime to n");
                }
            }

            //If we hit this point, everything must be valid. Set up x0
            x_prev = (x*x) % m;
        }

        /*! Calculates the next \f$ x_n \f$ and yields the template-defined number of bits
         
        \returns Result - The least significant bits of \f$ x_n \f$
        */
        result_type operator ()()
        {
            result_type out = 0;
            Integral x = x_prev;

            for(int i=0; i<bits; i++)
            {
                out = out | (cryptomath::mod2<Integral>(x) << i);
                x = x / 2;
            }

            //Increment state
            x_prev = (x_prev*x_prev) % m;

            return out;
        }
    };
}