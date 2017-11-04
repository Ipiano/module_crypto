#pragma once

#include "cryptomath.h"

#include <exception>
#include <array>
#include <unordered_map>
#include <vector>
#include <utility>
#include <string>

namespace affine
{
    template<class InputIterator, class OutputIterator>
    class affine
    template<class InputIterator, class OutputIterator>
    void encrypt(InputIterator message_begin, InputIterator message_end, OutputIterator cipher, 
                uint64_t a, uint64_t b)
    {
        vector<int> alphabet_inverse
        if(cryptomath::gcd(a, n) != 1)
            throw std::logic_error("gcd(" + to_string(a) + ", " + to_string(n) + ") != 1");

        for(; message_begin != message_end; message_begin++, cipher++)
        {
            *cipher = cryptomath::mod(a * (*message_begin) + b, n);
        }
    }

    template<class InputIterator, class OutputIterator>    
    void decrypt(InputIterator cipher_begin, InputIterator cipher_end, OutputIterator message, uint64_t a, uint64_t b, uint64_t n=26)
    {
        if(cryptomath::gcd(a, n) != 1)
            throw std::logic_error("gcd(" + to_string(a) + ", " + to_string(n) + ") != 1");

        uint64_t ainv = cryptomath::inverseMod(a, n);
        for(; cipher_begin != cipher_end; cipher_begin++, message++)
        {
            *message = cryptomath::mod(((*cipher_begin) - b) * ainv, n);
        }
    }
}