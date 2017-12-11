#include "../headers/des64.h"

#include <stdexcept>
#include <array>

using namespace std;

namespace des64
{
    namespace _internal
    {
        /*!
            \param[in] ri Right half of block i
            \param[in] ki Key for round i
            \returns uint64_t
        */
        uint64_t F(uint64_t ri, const uint64_t& ki)
        {
            DBG(cerr << "F(" << bin(ri) << ")" << endl);

            ri = permute<32, 48, uint64_t>(ri, EXPAND);
            DBG(cerr << "Permuted " << bin(ri) << endl);

            ri ^= ki;
            DBG(cerr << "XOR ki " << bin(ri) << endl);

            uint64_t C = 0;
            for(int i=7; i>=0; i--)
            {
                C |= ((S_BOXES[i][ri & 0x3F]) << (4 * (7-i)));
                ri >>= 6;
                DBG(cerr << "C " << bin(C) << endl);
            }

            C = permute<32, 32, uint64_t>(C, C_PERMUTE);
            DBG(cerr << "Permuted " << bin(C) << endl);

            return C;
        }        

        /*!
            \param[in] block The block to perform a round on
            \param[in] ki The key for the round
            \returns uint64_t - The resulting block
        */
        uint64_t round(uint64_t block, const uint64_t& ki)
        {
            uint64_t l = (block >> 32);
            uint64_t r = (block & 0xFFFFFFFF);

            return (r << 32) | (l ^ F(r, ki));
        }

        /*!
            A DES key contain 64 bits, but only 56
            are used. Every 8th bit is used as a parity of
            the 7 bits preceeding it. Every bytes of the
            DES key should contain an odd number of 1 bits.

            \param[in] key - The 64-bit key to check
            \returns bool - Whether or not the parity passed
        */
        bool parity_check(uint64_t key)
        {
            for(uint8_t i=0; i<8; i++)
            {
                uint8_t sum = 0;
                for(int j=0; j<8; j++)
                {
                    sum ^= (key & 1);
                    key >>= 1;
                }
                if(!sum) return false;
            }
            return true;
        }

        /*!
            Keys are generated from the initial 64-bit key by a combination
            of shifting the original key and then permuting the 58 non-parity bits

            \param[in] key - The 64-bit key to start with
            \returns array<uint64_t, 16> - 16 Round keys
            \throws logic_error : The key parity check failed
        */
        array<uint64_t, 16> make_keys(uint64_t key)
        {
            using namespace _internal;

            if(!parity_check(key))
                throw logic_error("key parity check failed");

            DBG(cerr << "Make keys from " << bin(key) << endl);
            array<uint64_t, 16> out;

            uint64_t c0d0 = permute<64, 56, uint64_t>(key, KEY_PERMUTE);
            DBG(cerr << "Permutation " << bin(c0d0) << endl);

            for(int i=0; i < 16; i++)
            {
                uint64_t c1d1 = ((rotate_left<28>(c0d0 >> 28, KEY_SHIFTS[i]) & 0xFFFFFFF) << 28) | (rotate_left<28>(c0d0 & 0xFFFFFFF, KEY_SHIFTS[i]) & 0xFFFFFFF);
                DBG(cerr << "CiDi\t" << bin(c1d1) << endl);

                out[i] = permute<56, 48, uint64_t>(c1d1, KEY_CHOOSE);
                DBG(cerr << "Ki\t" << bin(out[i]) << endl << endl);

                c0d0 = c1d1;
            }

            return out;
        }        
    }

    /*!
        The encryption process is as follows
            - Generate 16 round keys
            - Apply the initial permutation to the block
            - Encrypt the block with 16 rounds, using the keys in order
            - Swap the left and right halves of the block
            - Apply the inverse of the initial permutation

        \param[in] block The data to encrypt
        \param[in] key The 64-bit DES key
        \returns uint64_t - The encrypted block
        \throws logic_error : The key does not pass the parity check
    */
    uint64_t encrypt(uint64_t block, const uint64_t& key)
    {
        using namespace _internal;

        DBG(cerr << "Encrypt " << bin(block) << " : " << bin(key) << endl);
        array<uint64_t, 16> keys = make_keys(key);

        //Initial permute
        block = permute<64, 64, uint64_t>(block, IP);
        DBG(cerr << "IP " << bin(block) << endl);

        for(auto iter = keys.begin(); iter != keys.end(); iter++)
        {
            block = round(block, *iter);
            DBG(cerr << "Round " << bin(block) << endl);
        }

        //Flip block
        block = (block >> 32) | (block << 32);
        DBG(cerr << "Flip " << bin(block) << endl);

        //Inverse permute
        block = permute<64, 64, uint64_t>(block, IP_INV);
        DBG(cerr << "IP-1 " << bin(block) << endl);
        
        return block;
    }

    /*!
        The decryption process is as follows
            - Generate 16 round keys
            - Apply the initial permutation to the block
            - Decrypt the block with 16 rounds, using the keys in reverse order
            - Swap the left and right halves of the block
            - Apply the inverse of the initial permutation

        \param[in] block The data to decrypt
        \param[in] key The 64-bit DES key
        \returns uint64_t - The decrypted block
        \throws logic_error : The key does not pass the parity check
    */
    uint64_t decrypt(uint64_t block, const uint64_t& key)
    {
        using namespace _internal;

        //Inverse permute
        block = permute<64, 64, uint64_t>(block, IP);

        array<uint64_t, 16> keys = make_keys(key);
        for(auto iter = keys.rbegin(); iter != keys.rend(); iter++)
            block = round(block, *iter);
            
        //Flip block
        block = (block >> 32) | (block << 32);

        //Initial permute
        block = permute<64, 64, uint64_t>(block, IP_INV);

        return block;
    }
}