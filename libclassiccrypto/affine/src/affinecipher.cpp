#include "../headers/affinecipher.h"

#ifndef DEBUG
    #define DBG(a) 
#else
    #include <iostream>
    #include <iomanip>
    #define DBG(a) a
#endif

using namespace std;

namespace affine
{
    transformer::transformer(int64_t a, int64_t b, std::string alphabet) :
        _a(a), _b(b), _alphabet(alphabet)
    {
        int64_t n = _alphabet.size();

        if(!n)
            throw std::logic_error("alphabet is empty");

        //Check gcd(a, n)
        if(cryptomath::gcd(_a, n) != 1)
            throw std::logic_error("gcd(" + to_string(_a) + ", " + to_string(n) + ") != 1");

        _ainv = cryptomath::inverseMod(_a, n);
            
        //Build lookup table for alphabet
        for(int i=0; i<n; i++)
        {
            //Ensure each character only in alphabet once
            if(_alphabet_lookup.count(_alphabet[i]))
                throw std::logic_error("multiple \'" + string(1, _alphabet[i]) + "\' in alphabet");

            //Map char to index
            _alphabet_lookup[alphabet[i]] = i;
            DBG(cerr << alphabet[i] << " -> " << i << endl);
        }
    }

    string transformer::encrypt(const string& message)
    {
        int64_t n = _alphabet.size();
        string out = "";

        //Convert all characters in alphabet
        for(const char& c : message)
        {
            if(_alphabet_lookup.count(c))
            {
                out.push_back(_alphabet[cryptomath::mod(_a * _alphabet_lookup[c] + _b, n)]);
                DBG(cerr << _alphabet_lookup[c] << " * " << _a << " + " << _b << " (mod " << n << ")" << " = " << cryptomath::mod(_a * _alphabet_lookup[c] + _b, n));
                DBG(cerr << " -> " << _alphabet[cryptomath::mod(_a * _alphabet_lookup[c] + _b, n)] << endl);
            }
            else
                out.push_back(c);
        }

        return out;
    }

    string transformer::decrypt(const string& cipher)
    {
        int64_t n = _alphabet.size();
        string out = "";
        
        for(const char& c : cipher)
        {
            if(_alphabet_lookup.count(c))
            {
                out.push_back(_alphabet[cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n)]);
                DBG(cerr << "(" << _alphabet_lookup[c] << " - " << _b << ") * " << _ainv << " (mod " << n << ")" << " = " << cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n));
                DBG(cerr << " -> " << _alphabet[cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n)] << endl);
            }
            else
                out.push_back(c);
        }

        return out;
    }
}