#pragma once

#include <string>
#include <unordered_map>

namespace vigenere
{
    const std::string DEFAULT_ALPHABET = "!@#$%^&*()_-+={}[]:;\"\'<>?,./`~|\\ AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";
    
    class transformer
    {
        std::unordered_map<uint8_t, int64_t> _alphabet_lookup;
        std::unordered_map<uint8_t, int64_t> _key_alphabet_lookup;
        
        std::string _key;
        std::string _alphabet;  
        std::string _key_alphabet;        
        
        uint64_t _i = 0;

    public:
        transformer(const std::string& key, const std::string& key_alphabet = DEFAULT_ALPHABET, const std::string& alphabet = DEFAULT_ALPHABET);
        std::string encrypt(const std::string& message, bool reset = false);
        std::string decrypt(const std::string& cipher, bool reset = false);
    };
};