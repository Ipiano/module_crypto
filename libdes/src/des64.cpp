#include "../headers/des64.h"

#include <stdexcept>
#include <array>

using namespace std;

namespace des64
{
    namespace _internal
    {
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

        uint64_t round(uint64_t block, const uint64_t& ki)
        {
            uint64_t l = (block >> 32);
            uint64_t r = (block & 0xFFFFFFFF);

            return (r << 32) | (l ^ F(r, ki));
        }

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