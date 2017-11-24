/*! @file */ 
#include "../../catch.hpp"

#include "des4.h"

#include <cmath>
#include <utility>
#include <functional>

using namespace std;
using namespace des4;
using namespace _internal;

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

TEST_CASE("3 Round Crack")
{
    /*SECTION("Key = 000000000")
    {
        uint16_t key = 0b000000000;

        std::function<uint16_t(uint16_t)> encryptor = 
        [key](uint16_t block)
        {
            return encrypt(block, key, 3);  
        };

        REQUIRE(crack3(encryptor) == key);
    }*/
}