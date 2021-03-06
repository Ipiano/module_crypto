/*! @file */ 
#include "../../catch.hpp"

#include "des4.h"

#include <cmath>
#include <utility>
#include <functional>

using namespace std;
using namespace des4;
using namespace _internal;

/*!
    \test Tests that the internal functions for the simplified
    DES work correctly. Specifically
        - Expansion function
        - Foward key rotation
        - Backward key rotation
        - The F function
        - Rounds
*/
TEST_CASE("DES4 Internals")
{
    SECTION("Expansion")
    {
        REQUIRE(expand(0b011001) == 0b01010101);
        REQUIRE(expand(0b110101) == 0b11101001);
        REQUIRE(expand(0b010001) == 0b01000001);
        REQUIRE(expand(0b000110) == 0b00101010);
    };

    SECTION("Key Forward")
    {
        uint16_t key = 0b010010101;
        REQUIRE(next_key(key) == 0b00101010);
        REQUIRE(next_key(key) == 0b01010101);
        REQUIRE(next_key(key) == 0b10101010); 
        REQUIRE((key & 0x1FF) == 0b010101010);       
    };

    SECTION("Key Backward")
    {
        uint16_t key = 0b101010100;
        REQUIRE(prev_key(key) == 0b10101010);        
        REQUIRE(prev_key(key) == 0b01010101);
        REQUIRE(prev_key(key) == 0b00101010);
        REQUIRE((key & 0x1FF) == 0b100101010);               
    };

    SECTION("F")
    {
        REQUIRE(F(0b100110, 0b01100101) == 0b000100);
    };

    SECTION("Round")
    {
        REQUIRE(round(0b011100100110, 0b01100101) == 0b100110011000);
    }
};

/*!
    \test Tests that the simplified DES 3-round crack function works.
    Keys tested
        - 0
        - 141
        - 256
        - 010011010
*/
TEST_CASE("3 Round Crack")
{
    SECTION("Key = 0")
    {
        uint16_t key = 0;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key, 3);  
        };

        REQUIRE(crack3(encryptor) == key);
    }

    SECTION("Key = 141")
    {
        uint16_t key = 141;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key, 3);  
        };

        REQUIRE(crack3(encryptor) == key);
    }

    SECTION("Key = 265")
    {
        uint16_t key = 265;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key, 3);  
        };

        REQUIRE(crack3(encryptor) == key);
    }

    SECTION("Key = 010011010")
    {
        uint16_t key = 0b010011010;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key, 3);  
        };

        REQUIRE(crack3(encryptor) == key);
    }
}

/*!
    \test Tests that the simplified DES 4-round crack function works.
    Keys tested
        - 0
        - 141
        - 256
        - 010011010
        - 101110000
*/
TEST_CASE("4 Round Crack")
{
    SECTION("Key = 0")
    {
        uint16_t key = 0;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key);  
        };

        REQUIRE(crack4(encryptor) == key);
    }

    SECTION("Key = 141")
    {
        uint16_t key = 141;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key);  
        };

        REQUIRE(crack4(encryptor) == key);
    }

    SECTION("Key = 265")
    {
        uint16_t key = 265;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key);  
        };

        REQUIRE(crack4(encryptor) == key);
    }

    SECTION("Key = 010011010")
    {
        uint16_t key = 0b010011010;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key);  
        };

        REQUIRE(crack4(encryptor) == key);
    }

    SECTION("Key = 101110000")
    {
        uint16_t key = 0b101110000;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key);  
        };

        REQUIRE(crack4(encryptor) == key);
    }
}