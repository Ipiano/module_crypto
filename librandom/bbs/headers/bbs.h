#pragma once

#include <random>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <gmpxx.h>

#define gmpt(x) x.get_mpz_t()

namespace bbs
{
    template<class UIntType = uint32_t, uint32_t prime_reps = 25>
    class blum_blum_shub_engine
    {
        mpz_class n;
        mpz_class x_prev;
    public:
        typedef UIntType result_type;
        constexpr static result_type min() { return std::numeric_limits<result_type>::min(); }
        constexpr static result_type max() { return std::numeric_limits<result_type>::max(); }

        blum_blum_shub_engine(const mpz_class& p, const mpz_class& q, mpz_class x=-1)
        {
            //Check that p and q are (probably) prime
            if(! mpz_probab_prime_p(p.get_mpz_t(), prime_reps) ||
               ! mpz_probab_prime_p(q.get_mpz_t(), prime_reps))
            {
                throw std::domain_error("p or q is not prime");
            }

            const mpz_class CONST3(3);
            const mpz_class CONST4(4);
            
            //Check that p and q are congruent to 3 (mod 4)
            if(! mpz_congruent_p(gmpt(p), gmpt(CONST3), gmpt(CONST4)) ||
               ! mpz_congruent_p(gmpt(q), gmpt(CONST3), gmpt(CONST4)))
            {
                throw std::domain_error("p or q is not congruent to 3(mod 4)");                
            }

            //Set n
            n = p*q;  

            //Hacky log2(log2())
            uint64_t maxK = std::log2(mpz_sizeinbase(gmpt(n), 2));

            //Check that we can safely extract bits to fill result_type
            //and be cryptographically secure
            if(sizeof(result_type) > maxK)
            {
                std::string err = "cannot securely extract enough bits to fill result type(" + std::to_string(maxK) + ")";
                throw std::domain_error(err.c_str());
            }

            //If no seed chosen, use next prime after p + 1
            //guaranteed to be relatively prime to n because it's even
            if(x < 0)
            {
                mpz_nextprime(gmpt(x), gmpt(p));
                x = x+1;
            }
            else
            {
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
            //GMP doesn't seem to support 64-bit ints, so we
            //have to extract them a different (presumably slower) way
            result_type out;
            if(sizeof(result_type) > 4)
            {
                for(int i=0; i<sizeof(result_type)*8; i++)
                {
                    out |= (mpz_tstbit(gmpt(x_prev), i) << i);
                }
            }
            else
            {
                uint32_t ui = mpz_get_ui(gmpt(x_prev));
                out = *(reinterpret_cast<result_type*>(&ui + sizeof(ui) - sizeof(result_type)));
            }

            //Increment state
            x_prev = (x_prev * x_prev) % n;

            return out;
        }
    };
}