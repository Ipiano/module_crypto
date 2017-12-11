/*! \file */
#pragma once

#include <array>
#include <string>

#ifndef DEBUG
    /*! Removes verbose debug outputs from compiled result */
    #define DBG(a) 
#else
    #include <iostream>
    #include <iomanip>
    /*! Enables verbose debug outputs from compiled result */
    #define DBG(a) a
#endif

//! Namespace for full DES functionality
namespace des64
{
    //! Namespace for internal DES functions; these are not intended to be called by a user
    namespace _internal
    {
        /*! Converts a value to its binary representation

        \param[in] n The value to represent in binary
        \returns string - The binary representation of n
        Template arguments
            - class T - The type of value being converted. This should be some integral type
        */
        template<class T>
        std::string bin(T n)
        {
            std::string out = "";
            for(int i=0; i<sizeof(T)*8; i++, n>>=1)
                out = (n & 1 ? "1" : "0") + out;
            return out;
        }

        /*! \brief Typedef for a permutation used by the DES.

            Permutations are used in the DES to transform values
            bitwise. They may result in more or fewer bits than the
            input. 

            An example permutation [0, 2, 31, 23] would take a 32-bit number, 
            and output a 4-bit number consisting of bits 0, 2, 31, and 23 of the input

            Template arguments
                - uint64_t O - Number of bits in output type
        */
        template<uint64_t O>
        using permutation = std::array<int8_t, O>;

        //! Definition of the initial permutation
        const permutation<64> IP 
        {{58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7}};

        //! Definition of the initial permutation inverse
        const permutation<64> IP_INV 
        {{40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29, 
        36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27, 
        34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25}};

        //! Definition of the expansion function
        const permutation<48> EXPAND 
        {{32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13, 12,
        13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24,
        25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1}};

        //! Definition of the permuation of the C blocks before the application of the F function
        const permutation<32> C_PERMUTE 
        {{16, 7, 20, 21,
          29, 12, 28, 17,
          1, 15, 23, 26, 
          5, 18, 31, 10, 
          2, 8, 24, 14, 
          32, 27, 3, 9, 
          19, 13, 30, 6, 
          22, 11, 4, 25}};
        
        //! Definition of the key permutation to discard parity bits
        const permutation<56> KEY_PERMUTE 
        {{57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51,
        43, 35, 27, 19, 11, 3, 60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7,
        62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4}};

        //! Definition of how far to shift the key on each round
        const std::array<uint8_t, 16> KEY_SHIFTS
        {{1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1}};

        //! Definition of the permutation for getting bits out of the key to generate \f$ k_i \f$
        const permutation<48> KEY_CHOOSE
        {{14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4, 26, 8, 16, 7,
        27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 
        56, 34, 53, 46, 42, 50, 36, 29, 32}};

        //! Template for type representing an S-Box
        typedef std::array<uint8_t, 64> S_BOX;    

        /*! \brief The S-Boxes

        Since the S-Boxes are indexed first by the first and last bit of 6-bits, and then by
        the middle 4 bits, the following program was used to reorder the S-Boxes so they can
        be indexed into directly from an 8-bit value

        \code
        typedef array<uint8_t, 64> S_BOX;
        const array<S_BOX, 8> S_BOXES
        {{
            {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8, 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0, 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
            {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10, 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5, 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15, 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
            {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8, 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1, 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7, 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
            {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15, 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9, 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4, 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
            {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9, 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6, 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14, 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
            {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11, 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8, 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6, 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
            {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1, 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6, 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2, 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
            {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7, 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2, 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8, 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}
        }};

        int main()
        {
            cout << "typedef array<uint8_t, 64> S_BOX;" << endl <<
                    "const array<S_BOX, 8> S_BOXES" << "{{" << endl;
            for(int b = 0; b < 8; b++)
            {
                cout << "\t{{";
                for(int i = 0; i < 64; i++)
                {
                    int ip = (i & 0x20) | ((i & 0x1E) >> 1) | ((i & 1) << 4);
                    cout << (to_string((int)S_BOXES[b][ip]) + (i == 63 ? "" : ", ")) << (i && i < 63 && (i+1) % 16 == 0 ? "\n\t" : "");
                }
                cout << "}}" << (b == 7 ? "" : ",") << endl << endl;
            }
            cout << "}};" << endl;

            return 0;
        }
        \endcode
        */
        const std::array<S_BOX, 8> S_BOXES{{
                {{14, 0, 4, 15, 13, 7, 1, 4, 2, 14, 15, 2, 11, 13, 8, 1,
                3, 10, 10, 6, 6, 12, 12, 11, 5, 9, 9, 5, 0, 3, 7, 8,
                4, 15, 1, 12, 14, 8, 8, 2, 13, 4, 6, 9, 2, 1, 11, 7,
                15, 5, 12, 11, 9, 3, 7, 14, 3, 10, 10, 0, 5, 6, 0, 13}},
        
                {{15, 3, 1, 13, 8, 4, 14, 7, 6, 15, 11, 2, 3, 8, 4, 14,
                9, 12, 7, 0, 2, 1, 13, 10, 12, 6, 0, 9, 5, 11, 10, 5,
                0, 13, 14, 8, 7, 10, 11, 1, 10, 3, 4, 15, 13, 4, 1, 2,
                5, 11, 8, 6, 12, 7, 6, 12, 9, 0, 3, 5, 2, 14, 15, 9}},
        
                {{10, 13, 0, 7, 9, 0, 14, 9, 6, 3, 3, 4, 15, 6, 5, 10,
                1, 2, 13, 8, 12, 5, 7, 14, 11, 12, 4, 11, 2, 15, 8, 1,
                13, 1, 6, 10, 4, 13, 9, 0, 8, 6, 15, 9, 3, 8, 0, 7,
                11, 4, 1, 15, 2, 14, 12, 3, 5, 11, 10, 5, 14, 2, 7, 12}},
        
                {{7, 13, 13, 8, 14, 11, 3, 5, 0, 6, 6, 15, 9, 0, 10, 3,
                1, 4, 2, 7, 8, 2, 5, 12, 11, 1, 12, 10, 4, 14, 15, 9,
                10, 3, 6, 15, 9, 0, 0, 6, 12, 10, 11, 1, 7, 13, 13, 8,
                15, 9, 1, 4, 3, 5, 14, 11, 5, 12, 2, 7, 8, 2, 4, 14}},
        
                {{2, 14, 12, 11, 4, 2, 1, 12, 7, 4, 10, 7, 11, 13, 6, 1,
                8, 5, 5, 0, 3, 15, 15, 10, 13, 3, 0, 9, 14, 8, 9, 6,
                4, 11, 2, 8, 1, 12, 11, 7, 10, 1, 13, 14, 7, 2, 8, 13,
                15, 6, 9, 15, 12, 0, 5, 9, 6, 10, 3, 4, 0, 5, 14, 3}},
        
                {{12, 10, 1, 15, 10, 4, 15, 2, 9, 7, 2, 12, 6, 9, 8, 5,
                0, 6, 13, 1, 3, 13, 4, 14, 14, 0, 7, 11, 5, 3, 11, 8,
                9, 4, 14, 3, 15, 2, 5, 12, 2, 9, 8, 5, 12, 15, 3, 10,
                7, 11, 0, 14, 4, 1, 10, 7, 1, 6, 13, 0, 11, 8, 6, 13}},
        
                {{4, 13, 11, 0, 2, 11, 14, 7, 15, 4, 0, 9, 8, 1, 13, 10,
                3, 14, 12, 3, 9, 5, 7, 12, 5, 2, 10, 15, 6, 8, 1, 6,
                1, 6, 4, 11, 11, 13, 13, 8, 12, 1, 3, 4, 7, 10, 14, 7,
                10, 9, 15, 5, 6, 0, 8, 15, 0, 14, 5, 2, 9, 3, 2, 12}},
        
                {{13, 1, 2, 15, 8, 13, 4, 8, 6, 10, 15, 3, 11, 7, 1, 4,
                10, 12, 9, 5, 3, 6, 14, 11, 5, 0, 0, 14, 12, 9, 7, 2,
                7, 2, 11, 1, 4, 14, 1, 7, 9, 4, 12, 10, 14, 8, 2, 13,
                0, 15, 6, 12, 10, 9, 13, 0, 15, 3, 3, 5, 5, 6, 8, 11}}
        }};      

        /*! General permutation function
        
        This function takes a number and permutes it according to one
        of the permuation arrays defined for the DES

        Template arguments
            - uint64_t I - number of bits in input
            - uint64_t O - number of bits in output
            - class result - result type; must have at least O bits

        \param[in] block Data to permute
        \param[in] table Permutation table
        \returns result - The input permuted according to the table
        */
        template<uint64_t I, uint64_t O, class result = uint64_t>
        result permute(const uint64_t& block, const permutation<O>& table)
        {
            static_assert(sizeof(result)*8 >= O, "permute output has more bits than type");

            const uint64_t LEFT = (uint64_t)1 << (I-1);
            result out = 0;

            //DBG(std::cerr << "Permute " << I << " " << O << " " << bin(block) << std::endl);
            //DBG(std::cerr << "LEFT = " << bin(LEFT) << std::endl);

            for(int i = 0; i < O; i++)
            {
                out |= (((block & (LEFT >> (table[i] - 1))) >> (I - table[i])) << (O - i - 1));
                //DBG(std::cerr << bin(out) << std::endl);
            }

            return out;
        }

        /*! \brief Rotate left function

        Template arguments
            - uint64_t B - Number of bits to rotate

        \param[in] i Bits to rotate
        \param[in] n Distance to rotate
        \returns uint64_t i <<< n
        */
        template<uint64_t B>
        uint64_t rotate_left(const uint64_t& i, const uint64_t& n)
        {
            return (i << n) | (i >> (B - n));
        }

        //! Checks the parity bits of a DES key
        bool parity_check(uint64_t key);

        //! Performs a DES round
        uint64_t round(uint64_t block, const uint64_t& ki);

        //! Generates all 16 keys for the DES
        std::array<uint64_t, 16> make_keys(uint64_t key);

        //! The DES F function
        uint64_t F(uint64_t ri, const uint64_t& ki);
    }

    //! Encrypts a block with the key in 16 rounds
    uint64_t encrypt(uint64_t block, const uint64_t& key);

    //! Decrypts a block with the key in 16 rounds
    uint64_t decrypt(uint64_t block, const uint64_t& key);
}