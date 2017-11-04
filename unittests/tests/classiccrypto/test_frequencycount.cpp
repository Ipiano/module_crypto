/*! @file */ 
#include "../../catch.hpp"

#include "freq_count.h"

#include <cmath>
#include <utility>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace frequency;

TEST_CASE("Frequency count using array output")
{
    SECTION("Input non-string")
    {
        char results[256] = {0};

        SECTION("Empty")
        {
            int input[0];
            countFrequencies<int*, char*, int, char>(input, input+0, results,
                [](char& c){ c++; }, [](const int& i){ return i % 256; });

            for(int i=0; i<256; i++)
            {
                REQUIRE(results[i] == 0);
            }
        }

        SECTION("1 char")
        {
            int input[1] = {257};
            countFrequencies<int*, char*, int, char>(input, input+1, results,
                [](char& c){ c++; }, [](const int& i){ return i % 256; });

            for(int i=0; i<255; i++)
            {
                if(i != 1)
                    REQUIRE(results[i] == 0);
            }
            REQUIRE(results[1] == 1);
        }

        SECTION("Some data")
        {
            int input[5] = {256, 257, 258, 259, 258};
            countFrequencies<int*, char*, int, char>(input, input+5, results,
                [](char& c){ c++; }, [](const int& i){ return i % 256; });

            for(int i=0; i<255; i++)
            {
                if(i > 3)
                    REQUIRE(results[i] == 0);
            }
            REQUIRE(results[0] == 1);
            REQUIRE(results[1] == 1);
            REQUIRE(results[2] == 2);
            REQUIRE(results[3] == 1);
        }
    }

    SECTION("Input string")
    {
        char results[256] = {0};

        SECTION("Empty")
        {
            string input = "";
            countFrequencies<char*, char>(input, results, [](char& c){ c++; });

            for(int i=0; i<256; i++)
            {
                REQUIRE(results[i] == 0);
            }
        }

        SECTION("1 char")
        {
            string input = "a";
            countFrequencies<char*, char>(input, results, [](char& c){ c++; });

            for(int i=0; i<255; i++)
            {
                if(i != 'a')
                    REQUIRE(results[i] == 0);
            }
            REQUIRE(results['a'] == 1);
        }

        SECTION("Some data")
        {
            string input = "abcdb";
            countFrequencies<char*, char>(input, results, [](char& c){ c++; });

            for(int i=0; i<255; i++)
            {
                if(i > 'd' && i < 'a')
                    REQUIRE(results[i] == 0);
            }
            REQUIRE(results['a'] == 1);
            REQUIRE(results['b'] == 2);
            REQUIRE(results['c'] == 1);
            REQUIRE(results['d'] == 1);
        }
    }

    SECTION("Input stream")
    {
        char results[256] = {0};
        stringstream ss;

        SECTION("Empty")
        {
            string input = "";
            ss << input;
            countFrequencies<char*, char>(ss, results, [](char& c){ c++; });

            for(int i=0; i<256; i++)
            {
                REQUIRE(results[i] == 0);
            }
        }

        SECTION("1 char")
        {
            string input = "a";
            ss << input;
            countFrequencies<char*, char>(ss, results, [](char& c){ c++; });

            for(int i=0; i<255; i++)
            {
                if(i != 'a')
                    REQUIRE(results[i] == 0);
            }
            REQUIRE(results['a'] == 1);
        }

        SECTION("Some data")
        {
            string input = "abcdb";
            ss << input;
            countFrequencies<char*, char>(ss, results, [](char& c){ c++; });

            for(int i=0; i<255; i++)
            {
                if(i > 'd' && i < 'a')
                    REQUIRE(results[i] == 0);
            }
            REQUIRE(results['a'] == 1);
            REQUIRE(results['b'] == 2);
            REQUIRE(results['c'] == 1);
            REQUIRE(results['d'] == 1);
        }
    }
};

TEST_CASE("Frequency count using vector of custom type output")
{
    struct data
    {
        int count = 0;
    };

    SECTION("Input non-string")
    {
        vector<data> results(256);

        SECTION("Empty")
        {
            int input[0];
            countFrequencies<int*, vector<data>::iterator, int, data>(input, input+0, results.begin(),
                [](data& c){ c.count++; }, [](const int& i){ return i % 256; });

            for(int i=0; i<256; i++)
            {
                REQUIRE(results[i].count == 0);
            }
        }

        SECTION("1 char")
        {
            int input[1] = {257};
            countFrequencies<int*, vector<data>::iterator, int, data>(input, input+1, results.begin(),
                [](data& c){ c.count++; }, [](const int& i){ return i % 256; });

            for(int i=0; i<255; i++)
            {
                if(i != 1)
                    REQUIRE(results[i].count == 0);
            }
            REQUIRE(results[1].count == 1);
        }

        SECTION("Some data")
        {
            int input[5] = {256, 257, 258, 259, 258};
            countFrequencies<int*, vector<data>::iterator, int, data>(input, input+5, results.begin(),
                [](data& c){ c.count++; }, [](const int& i){ return i % 256; });

            for(int i=0; i<255; i++)
            {
                if(i > 3)
                    REQUIRE(results[i].count == 0);
            }
            REQUIRE(results[0].count == 1);
            REQUIRE(results[1].count == 1);
            REQUIRE(results[2].count == 2);
            REQUIRE(results[3].count == 1);
        }
    }

    SECTION("Input string")
    {
        vector<data> results(256);

        SECTION("Empty")
        {
            string input = "";
            countFrequencies<vector<data>::iterator, data>(input, results.begin(), [](data& c){ c.count++; });

            for(int i=0; i<256; i++)
            {
                REQUIRE(results[i].count == 0);
            }
        }

        SECTION("1 char")
        {
            string input = "a";
            countFrequencies<vector<data>::iterator, data>(input, results.begin(), [](data& c){ c.count++; });

            for(int i=0; i<255; i++)
            {
                if(i != 'a')
                    REQUIRE(results[i].count == 0);
            }
            REQUIRE(results['a'].count == 1);
        }

        SECTION("Some data")
        {
            string input = "abcdb";
            countFrequencies<vector<data>::iterator, data>(input, results.begin(), [](data& c){ c.count++; });

            for(int i=0; i<255; i++)
            {
                if(i > 'd' && i < 'a')
                    REQUIRE(results[i].count == 0);
            }
            REQUIRE(results['a'].count == 1);
            REQUIRE(results['b'].count == 2);
            REQUIRE(results['c'].count == 1);
            REQUIRE(results['d'].count == 1);
        }
    }

    SECTION("Input stream")
    {
        vector<data> results(256);
        stringstream ss;

        SECTION("Empty")
        {
            string input = "";
            ss << input;
            countFrequencies<vector<data>::iterator, data>(ss, results.begin(), [](data& c){ c.count++; });

            for(int i=0; i<256; i++)
            {
                REQUIRE(results[i].count == 0);
            }
        }

        SECTION("1 char")
        {
            string input = "a";
            ss << input;
            countFrequencies<vector<data>::iterator, data>(ss, results.begin(), [](data& c){ c.count++; });

            for(int i=0; i<255; i++)
            {
                if(i != 'a')
                    REQUIRE(results[i].count == 0);
            }
            REQUIRE(results['a'].count == 1);
        }

        SECTION("Some data")
        {
            string input = "abcdb";
            ss << input;
            countFrequencies<vector<data>::iterator, data>(ss, results.begin(), [](data& c){ c.count++; });

            for(int i=0; i<255; i++)
            {
                if(i > 'd' && i < 'a')
                    REQUIRE(results[i].count == 0);
            }
            REQUIRE(results['a'].count == 1);
            REQUIRE(results['b'].count == 2);
            REQUIRE(results['c'].count == 1);
            REQUIRE(results['d'].count == 1);
        }
    }
};