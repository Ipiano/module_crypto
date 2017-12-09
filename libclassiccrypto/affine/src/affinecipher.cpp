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
    /*! Initializes the transformer with a key pair and alphabet. If caseSensitive is false, then all
    upper-case text will be treated as lower-case during the transform (but it will be outputted in upper-case); in this
    case, all upper-case values in the alphabet will be changed to lower-case.

    Constraints
        - The alphabet must be non-empty
        - The alphabet must not contain duplicates
        - \f$ \alpha \f$ must be coprime with \f$ n \f$
    
    \param[in] a \f$ \alpha \f$
    \param[in] b \f$ \beta \f$
    \param[in] alphabet The alphabet to use (Defaults to affine::DEFAULT_ALPHABET)
    \param[in] caseSensitive Whether or not the transformer is case-sensitive (Defaults to true)

    \throws logic_error : The alphabet is empty
    \throws logic_error : The alphabet contains duplicates
    \throws logic_error : \f$ \alpha \f$ is not coprime to \f$ n \f$
    */
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
            
        //If case-insensitive, make alphabet lowercase
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

    /*! If the transformer is case-insensitive, then upper-case values
    will be transformed as if they were lower-case, but the resulting value will be outputted
    upper-case (if it is alphabetic). This may result in loss of some capitalization on decryption
    if the alphabet contains non-alphabetic characters.

    Any characters not in the alphabet will be unchanged

    \param[in] message The message to encrypt
    \returns string - The encrypted ciphertext
    */
    string transformer::encrypt(const string& message)
    {
        int64_t n = _alphabet.size();
        string out = "";

        //Convert all characters in alphabet
        for(const char& c_ : message)
        {
            char c = c_;
            bool caps = false;

            //If case-insensitive, use the lower-case version of the letter
            if(!_case && c_ >= 'A' && c_ <= 'Z')
            {
                c = tolower(c_);
                caps = true;
            }

            if(_alphabet_lookup.count(c))
            {
                //Calculate the result mod n
                char result = _alphabet[cryptomath::mod(_a * _alphabet_lookup[c] + _b, n)];

                //If it was originally uppercase, output upper case
                if(caps) result = toupper(result);
                out.push_back(result);

                DBG(cerr << _alphabet_lookup[c] << " * " << _a << " + " << _b << " (mod " << n << ")" << " = " << cryptomath::mod(_a * _alphabet_lookup[c] + _b, n));
                DBG(cerr << " -> " << _alphabet[cryptomath::mod(_a * _alphabet_lookup[c] + _b, n)] << endl);
            }
            else
                out.push_back(c_);
        }

        return out;
    }

    /*! If the transformer is case-insensitive, then upper-case values
    will be transformed as if they were lower-case, but the resulting value will be outputted
    upper-case (if it is alphabetic). This may result in loss of some capitalization from the original message
    if the alphabet contains non-alphabetic characters.

    Any characters not in the alphabet will be unchanged

    \param[in] message The message to decrypt
    \returns string - The decrypted message
    */
    string transformer::decrypt(const string& cipher)
    {
        int64_t n = _alphabet.size();
        string out = "";
        
        for(const char& c_ : cipher)
        {
            char c = c_;
            bool caps = false;

            //If case-insensitive, use the lower-case version of the letter
            if(!_case && c_ >= 'A' && c_ <= 'Z')
            {
                c = tolower(c_);
                caps = true;
            }

            if(_alphabet_lookup.count(c))
            {
                //Calulate the result mod n
                char result = _alphabet[cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n)];

                //If it was originally uppercase, output upper case
                if(caps) result = toupper(result);
                out.push_back(result);

                DBG(cerr << "(" << _alphabet_lookup[c] << " - " << _b << ") * " << _ainv << " (mod " << n << ")" << " = " << cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n));
                DBG(cerr << " -> " << _alphabet[cryptomath::mod((_alphabet_lookup[c] - _b) * _ainv, n)] << endl);
            }
            else
                out.push_back(c_);
        }

        return out;
    }
}