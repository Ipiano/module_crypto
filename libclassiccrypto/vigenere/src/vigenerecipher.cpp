#include "../headers/vigenerecipher.h"
#include "cryptomath.h"

#include <algorithm>
#include <cctype>

using namespace std;

namespace vigenere
{
    /*! Initializes a transformer with a key and alphabets for the key and texts. If caseSensitive is false, then all
    upper-case text will be treated as lower-case during the transform (but it will be outputted in upper-case); in this
    case, all upper-case values in the alphabet will be changed to lower-case.

    Constraints
        - The alphabets and key must be non-empty
        - Each alphabet must not contain the same character twice

    \param[in] key The key to use
    \param[in] key_alphabet Alphabet to use for mapping the key to integers
    \param[in] alphabet Alphabet to use for mapping texts to integers
    \param[in] caseSensitive Whether or not the transformer should be case sensitive

    \throws logic_error : One of the alphabets is empty
    \throws logic_error : The key is empty
    \throws logic_error : One of the alphabets contains the same character twice
    */
    transformer::transformer(const std::string& key, const string& key_alphabet, const string& alphabet, bool caseSensitive) :
         _key(key), _alphabet(alphabet), _key_alphabet(key_alphabet), _case(caseSensitive)
    {
        int64_t n = _alphabet.size();
        
        if(!n)
            throw std::logic_error("alphabet is empty");

        if(!_key_alphabet.size())
            throw std::logic_error("key alphabet is empty");

        if(!key.size())
            throw std::logic_error("key is empty");

        if(!_case)
        {
            transform(_alphabet.begin(), _alphabet.end(), _alphabet.begin(), [](unsigned char c){ return tolower(c); });
            transform(_key_alphabet.begin(), _key_alphabet.end(), _key_alphabet.begin(), [](unsigned char c){ return tolower(c); });
            transform(_key.begin(), _key.end(), _key.begin(), [](unsigned char c){ return tolower(c); });            
        }

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

    /*! The transformer will continue cycling the key with the character after the last used one. This
    allows the same transformer to be used for multiple lines of the same message. If the reset parameter
    is true, then the encryption will start with the first character of the key.

    If the transformer is case-insensitive, then upper-case values
    will be transformed as if they were lower-case, but the resulting value will be outputted
    upper-case (if it is alphabetic). This may result in loss of some capitalization on decryption
    if the alphabet contains non-alphabetic characters.

    \param[in] message The message to encrypt
    \param[in] reset Whether or not to start at the beginning of the key
    \returns string - The encrypted ciphertext
    */
    std::string transformer::encrypt(const std::string& message, bool reset)
    {
        int64_t n = _alphabet.size();
        
        if(reset) _i = 0;

        string out = "";
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
                char result = _alphabet[cryptomath::mod(_alphabet_lookup[c] + _key_alphabet_lookup[_key[_i++]], n)];
                if(caps) result = toupper(result);
                out += result;
                _i %= _key.size();
            }
            else
            {
                out += c_;
            }
        }

        return out;
    }

    /*! The transformer will continue cycling the key with the character after the last used one. This
    allows the same transformer to be used for multiple lines of the same message. If the reset parameter
    is true, then the decryptino will start with the first character of the key.

    If the transformer is case-insensitive, then upper-case values
    will be transformed as if they were lower-case, but the resulting value will be outputted
    upper-case (if it is alphabetic). This may result in loss of some capitalization from the original message
    if the alphabet contains non-alphabetic characters.

    \param[in] cipher The ciphertext to decrypt
    \param[in] reset Whether or not to start at the beginning of the key
    \returns string - The decrypted message
    */
    std::string transformer::decrypt(const std::string& cipher, bool reset)
    {
        int64_t n = _alphabet.size();
        
        if(reset) _i = 0;

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
                char result = _alphabet[cryptomath::mod(_alphabet_lookup[c] - _key_alphabet_lookup[_key[_i++]], n)];
                if(caps) result = toupper(result);
                out += result;
                _i %= _key.size();
            }
            else
            {
                out += c_;
            }
        }

        return out;
    }
}