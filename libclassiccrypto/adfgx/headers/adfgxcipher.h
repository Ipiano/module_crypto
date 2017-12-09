/*! \file */
#pragma once

#include <string>
#include <unordered_map>

/*! \brief Contains all library code for the ADFGX cipher

This includes only the ADFGX transformer type

The ADFGX cipher is a code which was developed by the Germans
during World War 1.
The cipher has four steps to encrypt a text
    - Using the matrix below, substitute a pair of letters from [adfgx] for each letter of the text
    - Write the substituted text under the key in column, going from left to right
    - Re-order the columns so the letters of the key are in alphabetical order
    - Write the columns (from top top bottom) from the left to the right
Decryption using the cipher follows the same pattern, but in reverse. To keep things secure during the war,
the initial substituion matrix was changed regularaly, along with the key.

Substitution matrix
\verbatim
    A  D  F  G  X
   --------------
A | p  g  c  e  n
D | b  q  o  z  r
F | s  l  a  f  t
G | m  d  v  i  w
X | k  u  y  x  h
\endverbatim

During the war, this cipher was thought to be very difficult to crack, but it was broken by the
French cryptanalyst Georges Painvin. After a couple of ciphertexts were recovered within a short period
of each other, he made the assumption that they had the same substitution matrix and key. With this assumption,
he tried writing out the text as if he were decrypting using various key lengths. If the beginnings of the
original messages were similar, then when the key length was correct, they would have a large number of matches
at the tops of the columns. After the key length was identified, the columns were ordered different ways,
and for each way the problem became a simple frequency analysis away from decryption.
*/
namespace adfgx
{
    /*! \brief The adfgx transformer

    Construct this object using a key for the adfgx cipher, and use
    it to encrypt and/or decrypt multiple texts using that key
    */
    class transformer
    {
        std::string _key; /*!< The key this transformer uses */

        //! Maps pairs of letters from [adfgx] to the letter they decrypt to
        static const std::unordered_map<char, std::unordered_map<char, char>> DECRYPT_TABLE;

        //! Maps single letters from the alphabet to the pair of letters from [adfgx] they encrypt to
        static const std::unordered_map<char, std::string> ENCRYPT_TABLE;

    public:
        //! Construts an ADFGX transformer with a specific keyword
        transformer(const std::string& key);

        //! Encrypts a string using the ADFGX cipher and the loaded key
        std::string encrypt(const std::string& message);

        //! Decrypts a string using the ADFGX cipher and the loaded key
        std::string decrypt(const std::string& cipher);
    };
}