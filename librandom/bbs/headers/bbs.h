#pragma once

#include <random>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <gmpxx.h>

#include "cryptomath.h"

#define gmpt(x) x.get_mpz_t()

namespace bbs
{
    template<class UIntType = uint32_t, uint64_t bits = 1, uint32_t prime_reps = 25>
    class blum_blum_shub_engine
    {
        mpz_class n;
        mpz_class x_prev;
    public:
        typedef UIntType result_type;
        constexpr static result_type min() { return std::numeric_limits<result_type>::min(); }
        constexpr static result_type max() { return std::numeric_limits<result_type>::max(); }

        blum_blum_shub_engine(mpz_class p, mpz_class q, mpz_class x=-1)
        {
            static_assert(sizeof(result_type)*8 >= bits, "Can't fit output bits it output type");

            p = abs(p);
            q = abs(q);

            //Check that p and q are (probably) prime
            if(! mpz_probab_prime_p(p.get_mpz_t(), prime_reps) ||
               ! mpz_probab_prime_p(q.get_mpz_t(), prime_reps))
            {
                throw std::domain_error("p or q is not prime");
            }
            
            //Check that p and q are congruent to 3 (mod 4)
            if(p % mpz_class(4) != mpz_class(3) ||
               q % mpz_class(4) != mpz_class(3))
            {
                throw std::domain_error("p or q is not congruent to 3(mod 4)");                
            }

            //Set n
            n = p*q;  

            //Hacky-looking log2(log2())
            uint64_t maxK = std::log2(mpz_sizeinbase(gmpt(n), 2));

            //Check that we can safely extract bits to fill result_type
            //and be cryptographically secure
            if(bits > maxK)
            {
                std::string err = "cannot securely extract enough bits to fill result type(" + std::to_string(maxK) + ")";
                throw std::domain_error(err.c_str());
            }

            //If no seed chosen, use (some prime after p) + 1
            if(x < 0)
            {
                mpz_nextprime(gmpt(x), gmpt(p));
                do
                {
                    mpz_nextprime(gmpt(x), gmpt(x));
                    x = x+1;
                }while(cryptomath::gcd(x, p) != 1 || cryptomath::gcd(x, q) != 1);
            }
            else
            {
                x = abs(x);

                mpz_class gcd;
                mpz_gcd(gmpt(gcd), gmpt(x), gmpt(n));
                if(gcd != 1)
                {
                    throw std::domain_error("x is not relatively prime to n");
                }
            }

            //If we hit this point, everything must be valid. Set up x0
            x_prev = (x * x) % n;
        }

        result_type operator ()()
        {
            result_type out;

            for(int i=0; i<bits; i++)
            {
                out |= (mpz_tstbit(gmpt(x_prev), i) << i);
            }

            //Increment state
            x_prev = (x_prev * x_prev) % n;

            return out;
        }
    };
}