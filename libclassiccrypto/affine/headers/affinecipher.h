/*! \file */
#pragma once

#include "cryptomath.h"

#include <unordered_map>
#include <string>

/*! \brief Contains all library code for affine ciphers

This includes only the affine cipher transformer type

Affine ciphers are a form of substitution cipher which takes the
plaintext to a ciphertext using the transform \f$ c = \alpha m + \beta \f$ (mod \f$ n \f$) where
    - \f$ c \f$ is the ciphertext
    - \f$ m \f$ is a character of the message
    - \f$ n \f$ is the size of the character set to use
    - \f$ \alpha \f$ is some value which is coprime with \f$ n \f$
    - \f$ \beta \f$ is any value (mod \f$ n \f$)
Text is decrypted with the transform \f$ m = (c - \beta)\alpha^{-1} \f$ (mod \f$ n \f$) where \f$ \alpha^{-1} \f$ is
the multiplicative inverse of \f$ \alpha \f$ mod \f$ n \f$

Characters are mapped to values mod \f$ n \f$ by their index in the alphabet. For example, if the alphabet is 'abcde', then
    - \f$ n = \f$ 5
    - 'a' maps to 0, 'b' maps to 1... 'e' maps to 4
    - All values except for 0 and multiples of 5 are valid for \f$ \alpha \f$ because 5 is prime

While this cipher is harder to crack than a simple shift cipher, it is still fairly trivial. If it is known what some plaintext
values map to in the ciphertext, then a linear system of two variables can be solve mod \f$ n \f$ to yield \f$ \alpha, \beta \f$.
If no mappings are known, a frequency analysis can be used to guess some.
*/
namespace affine
{
    /*! The default alphabet to use for affine transforms. It includes
        - a-z
        - A-Z
        - 0-9
        - Most special characters
    */
    const std::string DEFAULT_ALPHABET = "!@#$%^&*()_-+={}[]:;\"\'<>?,./`~|\\ AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";

    /*! \brief The affine transformer

    Construct this object using some \f$ \alpha, \beta \f$ and alphabet for an affine cipher, and use
    it to encrypt and/or decrypt multiple texts using that pair and alphabet
    */
    class transformer
    {
        //! Lookup table to map characters to their index in the alphabet
        std::unordered_map<uint8_t, int64_t> _alphabet_lookup;

        //! \f$ \alpha \f$
        int64_t _a;
        
        //! \f$ \beta \f$
        int64_t _b;

        //! \f$ \alpha^{-1} \f$ mod \f$ n \f$
        int64_t _ainv;

        //! The alphabet in use
        std::string _alphabet;

        //! Whether or not the transformer is case-sensitive
        bool _case;
        
    public:
        //! Construts an affine transformer with a specific \f$ \alpha, \beta \f$ and alphabet
        transformer(const int64_t& a, const int64_t& b, const std::string& alphabet = DEFAULT_ALPHABET, bool caseSensitive = true);
        
        //! Encrypts a text with the affine transform using the loaded key and alphabet
        std::string encrypt(const std::string& message);

        //! Decrypts a text with the affine transform using the loaded key and alphabet
        std::string decrypt(const std::string& message);
    };
}