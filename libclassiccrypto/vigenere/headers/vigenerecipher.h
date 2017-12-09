/*! \file */
#pragma once

#include <string>
#include <unordered_map>

/*! \brief Contains all library code for the vigenere cipher

This includes only the vigenere transformer type

The Vigenere cipher was invented during the 16th century, and is often attributed
to Vigenere. The ciper works similarly to a shift cipher, but instead of shifting 
each character by the same amount, a key is used to determine how far to shift each letter.

For example, if the key is '0 4 2 3', then the first letter of the message is shifted
by 0, the second by 4, the third by 2, and the fourth by 3. At this point, the key repeats,
so the fifth character is shifted by 0, the sixth by 4 and so on until the end of the message.

In general, the key is some text which is easy to remember. The text to be encrypted/decrypted and
key are mapped to numbers by their index in the respective alphabet.
For example, if the alphabet is 'abcde', then
    - 'a' maps to 0, 'b' maps to 1... 'e' maps to 4

This encryption method was thought to be secure through the twentieth cenurty, at which point Friedman
developed a generalized method for breaking it and similar ciphers. A common method of cracking the Vigenere
cipher involves comparing the ciphertext to itself, offset by varying amounts, to determine the key length. Once
the key length is determined, then sets of every nth character can be analyzed with a frequency analysis
to determine specific letters of the key.
*/
namespace vigenere
{
    /*! The default alphabet to use for the vigenere cipher. It includes
        - a-z
        - A-Z
        - 0-9
        - Most special characters
    */
    const std::string DEFAULT_ALPHABET = "!@#$%^&*()_-+={}[]:;\"\'<>?,./`~|\\ AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";
    
    /*! \brief The Vigenere transformer

    Construct this object using some key and two alphabets (One for the key, one for the texts) and use
    it to encrypt and/or decrypt multiple texts using that key and alphabets.
    */
    class transformer
    {
        //! Lookup to map text values to numbers
        std::unordered_map<uint8_t, int64_t> _alphabet_lookup;

        //! Lookup to map key values to numbers
        std::unordered_map<uint8_t, int64_t> _key_alphabet_lookup;
        
        //! The key
        std::string _key;

        //! The alphabet for encryption/decryption
        std::string _alphabet;  

        //! The alphabet for the key
        std::string _key_alphabet;  

        //! Whether or not the transformer is case-sensitive
        bool _case;      
        
        //! Which index of the key is to be used next
        uint64_t _i = 0;

    public:
        //! Constructs the transformer with a specific key and alphabets
        transformer(const std::string& key, const std::string& key_alphabet = DEFAULT_ALPHABET, const std::string& alphabet = DEFAULT_ALPHABET, bool caseSensitive = true);
        
        //! Encrypts a text using the loaded key and alphabets
        std::string encrypt(const std::string& message, bool reset = false);

        //! Decrypts a text using the loaded key and alphabets
        std::string decrypt(const std::string& cipher, bool reset = false);
    };
};