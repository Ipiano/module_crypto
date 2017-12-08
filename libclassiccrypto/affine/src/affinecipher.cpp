#include "../headers/affinecipher.h"
#include <algorithm>
#include <cctype>

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
    transformer::transformer(const int64_t& a, const int64_t& b, const std::string& alphabet, bool caseSensitive) :
        _a(a), _b(b), _alphabet(alphabet), _case(caseSensitive)
    {
        int64_t n = _alphabet.size();

        if(!n)
            throw std::logic_error("alphabet is empty");

        //Check gcd(a, n)
        if(cryptomath::gcd(_a, n) != 1)
            throw std::logic_error("gcd(" + to_string(_a) + ", " + to_string(n) + ") != 1");

        _ainv = cryptomath::inverseMod(_a, n);
            
        if(!_case)
            transform(_alphabet.begin(), _alphabet.end(), _alphabet.begin(), [](unsigned char c){ return tolower(c); });

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
        for(const char& c_ : message)
        {
            char c = c_;
            bool caps = false;

            if(!_case && c_ >= 'A' && c_ <= 'Z')
            {
                c = tolower(c_);
                caps = true;
            }

            if(_alphabet_lookup.count(c))
            {
                char result = _alphabet[cryptomath::mod(_a * _alphabet_lookup[c] + _b, n)];

                if(caps) result = toupper(result);
                out.push_back(result);

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
        
        for(const char& c_ : cipher)
        {
            char c = c_;
            bool caps = false;

            if(!_case && c_ >= 'A' && c_ <= 'Z')
            {
                c = tolower(c_);
                caps = true;
            }

            if(_alphabet_lookup.count(c))
            {
                char result = _alphabet[cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n)];
                if(caps) result = toupper(result);
                out.push_back(result);
                DBG(cerr << "(" << _alphabet_lookup[c] << " - " << _b << ") * " << _ainv << " (mod " << n << ")" << " = " << cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n));
                DBG(cerr << " -> " << _alphabet[cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n)] << endl);
            }
            else
                out.push_back(c);
        }

        return out;
    }
}