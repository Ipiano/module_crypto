/*! @file */ 
#pragma once

#include <random>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <gmpxx.h>

#include "cryptomath.h"

/*! Convenice macro for working with mpz_class types */
#define gmpt(x) x.get_mpz_t()

/*! Contains the Blum Blum Shub random engine */
namespace bbs
{
    /*! \brief A random engine implementing the Blum Blum Shub pseudorandom generator algorithm which satisfies the C++ UniformRandomBitGenerator concept
     *
     * The Blum Blum Shub pseudorandom generation algorithm is as follows
     *      -# Pick two primes p, q such that p and q are congruent to 3 mod 4
     *      -# Define M = p*q
     *      -# Pick a seed x such that p and q do not divide n
     *      -# x<SUB>0</SUB> = x * x (mod M)
     *      -# Repeatedly yield x<SUB>n</SUB> where x<SUB>n</SUB> = x<SUB>n-1</SUB><SUP>2</SUP> (mod M)
     *
     * This generator also satisfies the C++ concept UniformRandomBitGenerator, which means it can be used in
     * conjunction with any of the C++ random distributions to yield a sequence of random numbers that fit that distribution.
     *
     * Template arguments
     *      -# UIntType(class) - Unsigned result type to return yielded bits in
     *      -# Bits(uint64_t) - Number of least significant bits of each x<SUB>n</SUB> to return (default 1)
     *      -# prime_reps(uint32_t) - Number of repetitions to use in mpz_probab_prime test function (default 25)
     */
    template<class UIntType = uint32_t, uint64_t bits = 1, uint32_t prime_reps = 25>
    class blum_blum_shub_engine
    {
        mpz_class m; /*!< Product of p, q. Values are generated mod m */
        mpz_class x_prev; /*!< Previous calculated value x<SUB>n</SUB> */
    public:

        typedef UIntType result_type; /*!< Typedef of return type to satisfy UniformRandomBitGenerator */

        /*! Returns minimum possible value output by the generator
         * 
         * Needed to satisy UniformRandomBitGenerator
         *
         * @returns The smallest value that can be contained by the templated UIntType
         */
        constexpr static result_type min() { return std::numeric_limits<result_type>::min(); }

        /*! Returns maximum possible value output by the generator
         * 
         * Needed to satisy UniformRandomBitGenerator
         *
         * @returns The largest value that can be contained by the templated UIntType
         */
        constexpr static result_type max() { return std::numeric_limits<result_type>::max(); }

        /*! Constructs a new random generator with p, q and optional seed x
         *
         * It is recommended that gcd(totient(p), totient(q)) be small to maximize the number of values that
         * can be yielded before the engine cycles
         *
         * @param p The first prime to use to find M
         * @param q The second prime to use to find M
         * @param x Optional initial seed. If not specified, the valid number greater p which is a prime plus 1 is used
         *
         * @throws domain_error : p or q is not prime
         * @throws domain_error : p or q is not congruent to 3(mod 4)
         * @throws domain_error : The generator cannot securely yield the templated number of bits given p and q (Bits must be <= log2(log2(m)))
         * @throws domain_error : x is specified and it is not relatively prime to m
         */
        blum_blum_shub_engine(mpz_class p, mpz_class q, mpz_class x=-1)
        {
            static_assert(sizeof(result_type)*8 >= bits, "Can't fit output bits it output type");

            p = abs(p);
            q = abs(q);

            //Check that p and q are (probably) prime
            if(! cryptomath::isPrime(p, prime_reps) ||
               ! cryptomath::isPrime(q, prime_reps))
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

            uint64_t maxK = cryptomath::log2(cryptomath::log2(m));

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
                    /*! @todo Switch bbs engine next-prime to the one in cryptomath lib */
                    mpz_nextprime(gmpt(x), gmpt(x));
                    x = x+1;
                }while(x <= 1 && cryptomath::gcd(x, m) != 1);
            }
            else
            {
                x = abs(x);

                if(x <= 1 || cryptomath::gcd(x, m) != 1)
                {
                    throw std::domain_error("x is not relatively prime to n");
                }
            }

            //If we hit this point, everything must be valid. Set up x0
            x_prev = (x * x) % m;
        }

        /*! Calculates the next x<SUB>n</SUB> and yields the template-defined number of bits
         *
         * @returns UIntType with the least significant bits matching those in x<SUB>n</SUB>
         */
        result_type operator ()()
        {
            result_type out;
            mpz_class x = x_prev;

            for(int i=0; i<bits; i++)
            {
                out |= (x % 2) << i;
                x = x / 2;
            }

            //Increment state
            x_prev = (x_prev * x_prev) % m;

            return out;
        }
    };
}