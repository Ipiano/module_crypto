/*! @file */ 
#include "../../catch.hpp"

#include "vigenerecipher.h"

#include <cmath>
#include <utility>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*!
    \test Confirms that the vigenere cipher transformer will
    throw an exception if either alphabet is invalid
*/
TEST_CASE("Vigenere cipher errors")
{
    SECTION("Empty alphabet")
    {
        REQUIRE_THROWS(vigenere::transformer("hi", vigenere::DEFAULT_ALPHABET, ""));
    };

    SECTION("Empty key alphabet")
    {
        REQUIRE_THROWS(vigenere::transformer("hi", ""));
    };

    SECTION("Empty key")
    {
        REQUIRE_THROWS(vigenere::transformer(""));
    };

    SECTION("Duplicate in alphabet")
    {
        REQUIRE_THROWS(vigenere::transformer("hi", vigenere::DEFAULT_ALPHABET, "aabcedf"));
        REQUIRE_THROWS(vigenere::transformer("hi", vigenere::DEFAULT_ALPHABET, "abcdeff"));
    };

    SECTION("Duplicate in key alphabet")
    {
        REQUIRE_THROWS(vigenere::transformer("hi", "aabcedf"));
        REQUIRE_THROWS(vigenere::transformer("hi", "abcdeff"));
    };
};

/*!
    \test Confirms that the vigenere cipher transformer can be used
    to encrypt the text 'here is how it works' with the key 'vector'
*/
TEST_CASE("Vigenere cipher encrypt")
{
    SECTION("Alphabet = abcdefghijklmnopqrstuvwxyz")
    {
        string alphabet = "abcdefghijklmnopqrstuvwxyz";
        SECTION("key = vector")
        {
            vigenere::transformer vig("vector", alphabet, alphabet);
            REQUIRE(vig.encrypt("here is how it works") == "citx wj csy bh njvml");
        };
    };
};

/*!
    \test Confirms that the vigenere cipher transformer can be used
    to decrypt the text 'here is how it works' with the key 'vector'
*/
TEST_CASE("Vigenere cipher decrypt")
{
    SECTION("Alphabet = abcdefghijklmnopqrstuvwxyz")
    {
        string alphabet = "abcdefghijklmnopqrstuvwxyz";
        SECTION("key = vector")
        {
            vigenere::transformer vig("vector", alphabet, alphabet);
            REQUIRE(vig.decrypt("citx wj csy bh njvml") == "here is how it works");
        };
    };
};