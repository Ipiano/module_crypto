/*! @file */ 
#include "../../catch.hpp"

#include "cryptomath.h"

#ifdef CRYPTOMATH_GMP
#include <gmpxx.h>
#endif

using namespace std;
using namespace cryptomath;

/*!
    \test Tests that the factoring methods can all be used to factor various values
        - Prime values
            - 2, 3, 5, 7, 11, 13, 113, 163
        - Composite values
            - 1
            - \f$ 2^2*3*5*7*7 \f$
            - \f$ 2^8 \f$
            - \f$ 3^2*5 \f$
            - 11*13*17*23
            - 41*271

    Tests that mpz_class type can be used to factor the following values
        - 181
        - 11*13*17*23
        - \f$ 3^5*5*7*13^2*19*37*113 \f$
*/
TEST_CASE("The factoring functions")
{
    SECTION("Prime numbers")
    {
        vector<uint64_t> nums = {2, 3, 5, 7, 11, 13, 113, 163};
        for(const uint64_t& n : nums)
        {
            for(Factor_Method m = Factor_Method::Fermat; m <= Factor_Method::PollardP_1; m = (Factor_Method)((int)m+1))
            {
                vector<uint64_t> ans = factor(n, m);
                REQUIRE(ans == vector<uint64_t>{n});
            }
        }
    };

    SECTION("Non prime numbers")
    {
        vector<uint64_t> nums;
        vector<vector<uint64_t>> facs = {{1}, {2, 2, 3, 5, 7, 7}, {2, 2, 2, 2, 2, 2, 2, 2}, {3, 3, 5}, {11, 13, 17, 23}, {41, 271}};

        for(const vector<uint64_t>& n : facs)
        {
            uint64_t tot = 1;
            for(const uint64_t& f : n)
                tot *= f;
            nums.push_back(tot);
        }
        
        uint64_t i = 0;
        for(const uint64_t& n : nums)
        {
            for(Factor_Method m = Factor_Method::Fermat; m <= Factor_Method::PollardP_1; m = (Factor_Method)((int)m+1))
            {
                vector<uint64_t> ans = factor(n, m);
                REQUIRE(ans == facs[i]);
            }
            i++;
        }
    };

#ifdef CRYPTOMATH_GMP    
    SECTION("GMP compatible")
    {
        vector<mpz_class> nums;
        vector<vector<mpz_class>> facs = {{181},{11, 13, 17, 23},{3, 3, 3, 3, 3, 5, 7, 13, 13, 19, 37, 113}};

        for(const vector<mpz_class>& n : facs)
        {
            mpz_class tot = 1;
            for(const mpz_class& f : n)
                tot *= f;
            nums.push_back(tot);
        }
        
        uint64_t i = 0;
        for(const mpz_class& n : nums)
        {
            for(Factor_Method m = Factor_Method::Fermat; m <= Factor_Method::PollardP_1; m = (Factor_Method)((int)m+1))
            {
                vector<mpz_class> ans = factor(n, m);
                REQUIRE(ans == facs[i]);
            }
            i++;
        }
    };
#endif
}