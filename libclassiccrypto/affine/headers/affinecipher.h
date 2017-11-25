#pragma once

#include "cryptomath.h"

#include <unordered_map>
#include <string>

namespace affine
{
    const std::string DEFAULT_ALPHABET = "!@#$%^&*()_-+={}[]:;\"\'<>?,./`~|\\ AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";

    class transformer
    {
        std::unordered_map<uint8_t, int64_t> _alphabet_lookup;
        int64_t _a, _b;
        int64_t _ainv;
        std::string _alphabet;
        
    public:
        transformer(int64_t a, int64_t b, std::string alphabet = DEFAULT_ALPHABET);
        std::string encrypt(const std::string& message);
        std::string decrypt(const std::string& message);
    };
}