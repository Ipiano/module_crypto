#include "../headers/continuedfractions.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <exception>

using namespace std;

#ifndef DEBUG
    #define DBG(a) 
#else
    #include <iostream>
    #define DBG(a) a;
#endif

namespace cryptomath
{
namespace continuedfractions
{

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

vector<uint64_t> ftocf(uint64_t p, uint64_t q)
{
    DBG(cerr << "Continued fraction of " << p << " / " << q << endl)

    p = abs(p);
    q = abs(q);

    if(q == 0)
        throw invalid_argument("divide by 0");

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

vector<pair<uint64_t, uint64_t>> cftof(const vector<uint64_t>& cf)
{
    DBG(cerr << "Convert continued fraction to approximations ")
    DBG(for(uint64_t i = 0; i < 10 && i < cf.size(); cerr << cf[i++] << " "); cerr << endl;)

    vector<pair<uint64_t, uint64_t>> result;

    pair<uint64_t, uint64_t> p = pair<uint64_t, uint64_t>(0, 1);
    pair<uint64_t, uint64_t> q = pair<uint64_t, uint64_t>(1, 0);

    for(const uint64_t& a : cf)
    {
        p = pair<uint64_t, uint64_t>(p.second, a * p.second + p.first);
        q = pair<uint64_t, uint64_t>(q.second, a * q.second + q.first);

        DBG(cerr << p.second << " / " << q.second << endl;)

        result.emplace_back(p.second, q.second);
    }

    return result;
}

double cftod(const std::vector<uint64_t>& cf)
{
    DBG(cerr << "Continued fraction to double ");
    DBG(for(int i=0; i < 10 && i < cf.size(); cerr << cf[i++] << " "); cerr << endl;)

    double x = cf.back();
    for(auto iter = cf.rbegin()+1; iter != cf.rend(); iter++)
    {
        x = (*iter + 1.0/x);
    }

    return x;
}

}
}