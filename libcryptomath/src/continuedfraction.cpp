/*!
 * @file 
 */ 

#include "../headers/continuedfractions.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace std;

#ifndef DEBUG
    /*! Removes verbose debug outputs from compiled result */
    #define DBG(a) 
#else
    #include <iostream>
    /*! Enables verbose debug outputs from compiled result */
    #define DBG(a) a;
#endif

namespace cryptomath
{
namespace continuedfractions
{

/*! Takes a real value and converts it to a continued fraction, represented as [i; f, f, f...]
 * where i is the integer part of the fraction, and f1 - fn are the integer values added on the layers
 * of the fraction. The first value in the array returned is i, and the remaining values are f1 - fn, in order.
 *
 * The result may contain only an integer value
 *
 * If the result has a cycle, it will not be detected. If you need cycle detection in a simple square root, use
 * ::sqrttocf
 *  
 * @param[in] d The real value to convert
 * @param[in] max_iterations Maximum number of iterations to run if the fraction does not terminate sooner
 *
 * @returns vector<uint64_t> - The continued fraction, including the integer part
 */
vector<uint64_t> dtocf(double d, const uint64_t& max_iterations)
{
    DBG(cerr << "Continued fraction of " << d << endl)

    d = abs(d);
    uint64_t a = floor(d);
    double x = d;

    vector<uint64_t> result{a};

    int64_t iterations = 0;
    while(x != a && iterations++ < max_iterations)
    {
        x = 1.0/(x - a);
        a = floor(x);

        DBG(cerr << "X: " << x << " -- " << a << endl);

        result.emplace_back(a);
    }

    return result;
}

/*! Finds the continued fraction of a rational as an integer i and a finite set of continued fraction coefficients.
 *
 * @param[in] p The top of the rational to convert
 * @param[in] q The bottom of the rational to convert
 *
 * @returns vector<uint64_t> - The continued fraction, including the integer part
 */
vector<uint64_t> ftocf(uint64_t p, uint64_t q)
{
    DBG(cerr << "Continued fraction of " << p << " / " << q << endl)

    if(q == 0)
        throw logic_error("divide by 0");

    uint64_t a = p/q;
    p = p%q;

    vector<uint64_t> result{a};

    int64_t iterations = 0;
    while(p)
    {
        swap(p, q);
        a = p/q;
        p = p%q;

        DBG(cerr << "X: " << p << " / " << q << " -- " << a << endl);

        result.emplace_back(a);
    }

    return result;
}

/*! Finds the continued fraction of sqrt(n) as an integer i and repeating continued fraction coefficients. The first value in the returned continued
 * fraction is the integer portion, and the remaning values will repeat infinitely. The end of the period is the first
 * value in the repeating fraction a such that a == 2*i
 *  
 * @param[in] n The value to convert the square root of
 *
 * @returns vector<uint64_t> The continued fraction, including the integer part. Everything except the integer part repeats infinitely
 */
std::vector<uint64_t> sqrttocf(uint64_t n)
{
    DBG(cerr << "Continued fraction of sqrt(" << n << ")" << endl)

    double x = sqrt(n);
    uint64_t a = floor(x);
    uint64_t intPart = a;

    vector<uint64_t> result{a};

    //No continuing fraction for perfect squares
    if(abs(x - a) > 0.0001)
    {
        while(intPart != a/2)
        {
            x = 1.0/(x - a);
            a = floor(x);

            DBG(cerr << "X: " << x << " -- " << a << endl);

            result.emplace_back(a);
        }
    }

    return result;
}

/*! Generates all p/q approximations of the continued fraction using the following formulas
 *      - p(k) = a(k) * p(k-1) + p(k-2)
 *      - q(k) = a(k) * q(k-1) + q(k-2)
 *  Where
 *      - p(-2) = 0, p(-1) = 1
 *      - q(-2) = 1, q(-1) = 0
 *  
 * By default, all values will be used once. If repetitions is set to n (n != 0), then the first
 * n times the loop hits the last value in the input, the loop will jump back to the second value.
 *
 * For example: [1, 2, 3, 2] with 0 repetitions (default) will process 1, 2, 3, 2. With 2 repetitions, it will process 1, 2, 3, 2, 2, 3, 2, 2, 3, 2
 *
 * @param[in] cf The continued fraction to convert
 * @param[in] repetitions The number of times to repeat the non-integer portion beyond the first time.
 *
 * @returns vector<pair<uint64_t, uint64_t>> - All p/q approximations generated by the cf
 */
vector<pair<uint64_t, uint64_t>> cftof(const std::vector<uint64_t>& cf, uint64_t repetitions)
{
    DBG(cerr << "Convert continued fraction to approximations ")
    DBG(for(uint64_t i = 0; i < 10 && i < cf.size(); cerr << cf[i++] << " "); cerr << endl;)

    vector<pair<uint64_t, uint64_t>> result;

    pair<uint64_t, uint64_t> p = pair<uint64_t, uint64_t>(0, 1);
    pair<uint64_t, uint64_t> q = pair<uint64_t, uint64_t>(1, 0);

    auto end = cf.end();
    auto iter = cf.begin();
    while(iter != end)
    {
        p = pair<uint64_t, uint64_t>(p.second, (*iter) * p.second + p.first);
        q = pair<uint64_t, uint64_t>(q.second, (*iter) * q.second + q.first);

        DBG(cerr << p.second << " / " << q.second << endl;)

        result.emplace_back(p.second, q.second);

        iter++;

        //Repeat all but integer portion for sqrt continued fractions
        if(iter == end && repetitions)
        {
            DBG(cerr << "Repeat" << endl)
            iter = cf.begin()+1;
            repetitions--;
        }
    }

    return result;
}

/*! Generates a double value from a continued fraction form by using the formula x = i + 1/x for each
 * value in the continued fraction in reverse order.
 *
 * By default, all values will be used once. If repetitions is set to n (n != 0), then the first
 * n times the loop hits the second value in the input, the loop will jump back to the end.
 *
 * For example: [1, 2, 3, 2] with 0 repetitions (default) will process 2, 3, 2, 1. With 2 repetitions, it will process 2, 3, 2, 2, 3, 2, 2, 3, 2, 1.
 *  
 * @param[in] cf The continued fraction to convert
 * @param[in] repetitions The number of times to repeat the non-integer portion beyond the first time. 
 *
 * @returns double - The real value
 */
double cftod(const std::vector<uint64_t>& cf, uint64_t repetitions)
{
    DBG(cerr << "Continued fraction to double ");
    DBG(for(int i=0; i < 10 && i < cf.size(); cerr << cf[i++] << " "); cerr << endl;)

    double x = cf.back();
    auto end = cf.rend();
    auto iter = cf.rbegin()+1;

    //Handle case of a single value repeated
    if(iter == end-1 && repetitions)
    {
        iter = cf.rbegin();
        repetitions--;
    }

    while(iter != end)
    {
        x = (*iter + 1.0/x);

        iter++;

        if(iter == end-1 && repetitions)
        {
            iter = cf.rbegin();
            repetitions--;
        }
    }

    return x;
}

}
}