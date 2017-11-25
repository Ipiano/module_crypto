#include "../headers/vigenerecipher.h"
#include "cryptomath.h"

using namespace std;

namespace vigenere
{
    transformer::transformer(const std::string& key, const string& key_alphabet, const string& alphabet) :
         _key(key), _alphabet(alphabet), _key_alphabet(key_alphabet)
    {
        int64_t n = _alphabet.size();
        
        if(!n)
            throw std::logic_error("alphabet is empty");

        if(!_key_alphabet.size())
            throw std::logic_error("key alphabet is empty");

        if(!key.size())
            throw std::logic_error("key is empty");

        //Build lookup table for alphabet
        for(int i=0; i<n; i++)
        {
            //Ensure each character only in alphabet once
            if(_alphabet_lookup.count(_alphabet[i]))
                throw std::logic_error("multiple \'" + string(1, _alphabet[i]) + "\' in alphabet");

            //Map char to index
            _alphabet_lookup[_alphabet[i]] = i;
        }

        n = _key_alphabet.size();
        //Build lookup table for keyalphabet
        for(int i=0; i<n; i++)
        {
            //Ensure each character only in alphabet once
            if(_key_alphabet_lookup.count(_key_alphabet[i]))
                throw std::logic_error("multiple \'" + string(1, _key_alphabet[i]) + "\' in key alphabet");

            //Map char to index
            _key_alphabet_lookup[_key_alphabet[i]] = i;
        }
    }

    std::string transformer::encrypt(const std::string& message, bool reset)
    {
        int64_t n = _alphabet.size();
        
        if(reset) _i = 0;

        string out = "";
        for(const char& c : message)
        {
            if(_alphabet_lookup.count(c))
            {
                out += _alphabet[cryptomath::mod(_alphabet_lookup[c] + _key_alphabet_lookup[_key[_i++]], n)];
                _i %= _key.size();
            }
            else
            {
                out += c;
            }
        }

        return out;
    }

    std::string transformer::decrypt(const std::string& cipher, bool reset)
    {
        int64_t n = _alphabet.size();
        
        if(reset) _i = 0;

        string out = "";
        for(const char& c : cipher)
        {
            if(_alphabet_lookup.count(c))
            {
                out += _alphabet[cryptomath::mod(_alphabet_lookup[c] - _key_alphabet_lookup[_key[_i++]], n)];
                _i %= _key.size();
            }
            else
            {
                out += c;
            }
        }

        return out;
    }
}