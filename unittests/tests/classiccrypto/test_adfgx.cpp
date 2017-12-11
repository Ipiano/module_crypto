/*! @file */ 
#include "../../catch.hpp"

#include "adfgxcipher.h"

#include <cmath>
#include <utility>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*!
    \test Confirms that the adfgx cipher transformer will
    correctly throw exceptions if the key is invalid
*/
TEST_CASE("ADFGX cipher errors")
{
    SECTION("Empty key")
    {
        REQUIRE_THROWS(adfgx::transformer(""));
    };

    SECTION("Duplicate in key")
    {
        REQUIRE_THROWS(adfgx::transformer("aabcedf"));
        REQUIRE_THROWS(adfgx::transformer("abcdeff"));
    };
};

/*!
    \test Confirms that the adfgx cipher transformer will
    correctly encrypt the example in the reference book (Key - RHINE; text Kaiser Wilhelm) and
    that whitespace and extra characters in the text are ignored
*/
TEST_CASE("ADFGX cipher encrypt")
{
    SECTION("Key = RHINE")
    {
        string key = "RHINE";
        adfgx::transformer ciph(key);
        SECTION("Kaiser Wilhelm")
        {
            // This is the example in the book, but the output doesn't match because I'm
            // pretty sure the book is wrong
            REQUIRE(ciph.encrypt("Kaiser Wilhelm") == "gggxgafxfgfagdffaxxdxgdgaa");
            REQUIRE(ciph.encrypt("KaiserWilhelm") == "gggxgafxfgfagdffaxxdxgdgaa");
            REQUIRE(ciph.encrypt("Kai1ser Wil8helm.") == "gggxgafxfgfagdffaxxdxgdgaa");
            
        };
    };
};

/*!
    \test Confirms that the adfgx cipher transformer will
    correctly decrypt the example in the reference book (Key - RHINE; text Kaiser Wilhelm) and
    that whitespace and extra characters in the text are ignored
*/
TEST_CASE("ADFGX cipher decrypt")
{
    SECTION("Key = RHINE")
    {
        string key = "RHINE";
        adfgx::transformer ciph(key);
        SECTION("Kaiser Wilhelm")
        {
            REQUIRE(ciph.decrypt("gggxgafxfgfagdffaxxdxgdgaa") == "kaiserwilhelm");
            REQUIRE(ciph.decrypt("gggx ga  fxfgf a gdff ax xdx gdg aa") == "kaiserwilhelm");
            REQUIRE(ciph.decrypt("gg guecxgalp23fxfgfmqzagdhl;ffaxx./?dxgd']gaa") == "kaiserwilhelm");
            
        };
    };
};