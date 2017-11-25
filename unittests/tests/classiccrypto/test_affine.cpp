/*! @file */ 
#include "../../catch.hpp"

#include "affinecipher.h"

#include <cmath>
#include <utility>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

TEST_CASE("Affine cipher errors")
{
    SECTION("Empty alphabet")
    {
        REQUIRE_THROWS(affine::transformer(3, 5, ""));
    };

    SECTION("Duplicate in alphabet")
    {
        REQUIRE_THROWS(affine::transformer(3, 5, "aabcedf"));
        REQUIRE_THROWS(affine::transformer(3, 5, "abcdeff"));
    };

    SECTION("A, N not coprime")
    {
        REQUIRE_THROWS(affine::transformer(2, 5, "abcdef"));
        REQUIRE_THROWS(affine::transformer(13, 4, "abcdefghijklmnopqrstuvwxyz"));
    };
};

TEST_CASE("Affine cipher encrypt")
{
    SECTION("Alphabet = abcdefghijklmnopqrstuvwxyz")
    {
        string alphabet = "abcdefghijklmnopqrstuvwxyz";
        SECTION("a = 11, b = 5")
        {
            affine::transformer aff(11, 5, alphabet);
            REQUIRE(aff.encrypt("testa") == "gxvgf");
        };

        SECTION("a = 15, b = 2")
        {
            affine::transformer aff(15, 2, alphabet);
            REQUIRE(aff.encrypt("testb") == "bkmbr");
        };

        SECTION("a = 19, b = 22")
        {
            affine::transformer aff(19, 22, alphabet);
            REQUIRE(aff.encrypt("testc") == "tuati");
        };
    };

    SECTION("Alphabet = abcdefghijklmnopqrstuvwxyz1234567890")
    {
        string alphabet = "abcdefghijklmnopqrstuvwxyz1234567890";
        SECTION("a = 5, b = 23")
        {
            affine::transformer aff(5, 23, alphabet);
            REQUIRE(aff.encrypt("test1") == "khfkj");
        };

        SECTION("a = 13, b = 2")
        {
            affine::transformer aff(13, 2, alphabet);
            REQUIRE(aff.encrypt("test2") == "8su84");
        };

        SECTION("a = 23, b = 26")
        {
            affine::transformer aff(23, 26, alphabet);
            REQUIRE(aff.encrypt("test3") == "6ki6w");
        };
    };
};

TEST_CASE("Affine cipher decrypt")
{
    SECTION("Alphabet = abcdefghijklmnopqrstuvwxyz")
    {
        string alphabet = "abcdefghijklmnopqrstuvwxyz";
        SECTION("a = 11, b = 5")
        {
            affine::transformer aff(11, 5, alphabet);
            REQUIRE(aff.decrypt("gxvgf") == "testa");
        };

        SECTION("a = 15, b = 2")
        {
            affine::transformer aff(15, 2, alphabet);
            REQUIRE(aff.decrypt("bkmbr") == "testb");
        };

        SECTION("a = 19, b = 22")
        {
            affine::transformer aff(19, 22, alphabet);
            REQUIRE(aff.decrypt("tuati") == "testc");
        };
    };

    SECTION("Alphabet = abcdefghijklmnopqrstuvwxyz1234567890")
    {
        string alphabet = "abcdefghijklmnopqrstuvwxyz1234567890";
        SECTION("a = 5, b = 23")
        {
            affine::transformer aff(5, 23, alphabet);
            REQUIRE(aff.decrypt("khfkj") == "test1");
        };

        SECTION("a = 13, b = 2")
        {
            affine::transformer aff(13, 2, alphabet);
            REQUIRE(aff.decrypt("8su84") == "test2");
        };

        SECTION("a = 23, b = 26")
        {
            affine::transformer aff(23, 26, alphabet);
            REQUIRE(aff.decrypt("6ki6w") == "test3");
        };
    };
};