#include "../headers/des4.h"

#include <random>
#include <set>
#include <stdexcept>
#include <algorithm>

#ifndef DEBUG
    #define DBG(a) 
#else
    #include <iostream>
    #include <iomanip>
    #define DBG(a) a
#endif

using namespace std;

namespace des4
{
    namespace _internal
    {
        template<class T>
        string bin(T n)
        {
            string out = "";
            for(int i=0; i<sizeof(T)*8; i++, n>>=1)
                out = (n & 1 ? "1" : "0") + out;
            return out;
        }

        uint8_t expand(const uint8_t& n)
        { 
            //          1, 2              3 - Stay           3 - Swap
            return ((n & 0x30) << 2) | ((n & 0x8) << 1) | ((n & 0x8) >> 1) |
            //        4 - Stay              4 - Swap         5, 6
                    ((n & 0x4) << 1) | ((n & 0x4) << 3) | (n & 0x3);
        }

        uint8_t next_key(uint16_t& key)
        {
            //Rotate left so next key is bottom 8 bits
            key = (key << 1) | ((key & 0x1FF) >> 8);

            //Extract bottom 8 bits
            return 0xFF & key;
        }

        uint8_t prev_key(uint16_t& key)
        {
            //Rotate right so next key is bottom 8 bits
            key = (key >> 1) | (key << 8);

            //Extract bottom 8 bits
            return 0xFF & key;
        }

        uint8_t F(uint8_t r, const uint8_t& key)
        {
            DBG(cerr << "F(" << bin(r) << ")" << endl);

            r = expand(r);
            DBG(cerr << "E = " << bin(r) << endl);

            r ^= key;
            DBG(cerr << "E^k = " << bin(r) << endl);

            DBG(cerr << bin((uint8_t)((r & 0xF0) >> 4)) << " : " << bin((uint8_t)(r & 0xF)) << endl);
            DBG(cerr << "S1 -> " << bin(S_BOXES[0][(uint8_t)((r & 0xF0) >> 4)]) << endl);
            DBG(cerr << "S2 -> " << bin(S_BOXES[1][(uint8_t)(r & 0xF)]) << endl);
            return S_BOXES[0][(uint8_t)((r & 0xF0) >> 4)] << 3 | S_BOXES[1][(uint8_t)(r & 0xF)];
        }        

        uint16_t round(const uint16_t& block, const uint8_t& key)
        {
            DBG(cerr << "Round " << bin(block) << " : " << bin(key) << endl);

            uint8_t l = (block & 0xFC0) >> 6;
            uint8_t r = block & 0x3F;

            uint8_t f = F(r, key);

            DBG(cerr << "f -> " << bin(f) << endl);
            DBG(cerr << bin((uint16_t)(r << 6)) << " : " << bin((uint8_t)(f ^ l)) << endl);
            return (r << 6) | (f ^ l);
        }
    }

    uint16_t encrypt(uint16_t block, uint16_t key, const uint64_t& rounds)
    {
        using namespace _internal;

        DBG(cerr << "Encrypt " << bin(block) << " : " << bin(key) << endl);
        
        //Rotate key to right to key -1 to make extraction of subkeys easier
        key = (key >> 2) | (key << 7);
        DBG(cerr << "Adjust key -> 2:" << bin(key) << endl);

        //Do rounds
        for(int i=0; i<rounds; i++)
        {
            block = round(block, next_key(key));
            DBG(cerr << "Round -> " << bin(block) << " : " << bin(key) << endl);
        }
        DBG(cerr << endl);   
        
        return block & 0xFFF;
    }

    uint16_t decrypt(uint16_t block, uint16_t key, const uint64_t& rounds)
    {
        using namespace _internal;

        DBG(cerr << "Decrypt " << bin(block) << " : " << bin(key) << endl);
        
        //Rotate key to right to key rounds+1 to make extraction of subkeys easier
        uint8_t final_key = rounds % 9-1;
        key = (key >> (9-final_key)) | (key << final_key);
        DBG(cerr << "Adjust key -> " << (int)final_key << ": " << bin(key) << endl);
        
        //Swap block
        block = (block << 6) | ((block & 0xFC0) >> 6);
        DBG(cerr << "Flip block: " << bin(block) << endl);

        //Do rounds
        for(int i=0; i<rounds; i++)
        {
            block = round(block, prev_key(key));
            DBG(cerr << "Round -> " << bin(block) << " : " << bin(key) << endl);            
        }  

        //Swap block
        block = (block << 6) | ((block & 0xFC0) >> 6);
        DBG(cerr << "Flip block: " << bin(block) << endl);

        DBG(cerr << endl);

        return block & 0xFFF;
    }

    uint16_t crack3(function<uint16_t(uint16_t)> des3)
    {
        using namespace _internal;

        DBG(cerr << hex << "Cracking DES3" << endl);

        //Build lookup table for xor combinations
        DBG(cerr << "Build xor lookup" << endl);
        array<vector<pair<uint8_t, uint8_t>>, 16> xor_lookup;
        for(uint8_t i = 0; i < 16; i++)
            for(uint8_t j = 0; j < 16; j++)
                xor_lookup[i^j].emplace_back(i, j);

        //Random generator to get test cases
        mt19937 reng;
        uniform_int_distribution<> dist(0, 0xFFF);

        //Candidate sets or left and right parts of k
        //Repeat until these both have 1 item left
        vector<uint8_t> k1 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
        vector<uint8_t> k2 = k1;

        while(k1.size() != 1 || k2.size() != 1)
        {
            DBG(cerr << dec << k1.size() << " potential key lefts" << endl);
            DBG(cerr << k2.size() << " potential key rights" << hex << endl);

            //Should never happen
            if(k1.size() == 0 || k2.size() == 0)
                throw logic_error("key left or key right has 0 options");

            //Pick two random inputs with R1 = R1*
            uint16_t i = dist(reng);
            uint16_t is = (dist(reng) & 0xFC0) | (i & 0x3F);
            DBG(cerr << "LR1 = " << i << " : LR1* = " << is << endl);

            //Get their outputs
            uint16_t o = des3(i);
            uint16_t os = des3(is);
            DBG(cerr << "LR3 = " << o << " : LR3* = " << os << endl);

            //Compute r4' ^ l1'
            uint16_t rp4lp1 = (((i ^ is) & 0xFc) >> 6) ^ ((o ^ os) & 0x3F);
            DBG(cerr << "R4\' ^ L1\' = " << rp4lp1 << endl);

            //Find output XORs from S-Boxes
            uint8_t s1o = (rp4lp1 & 0x70) >> 3;
            uint8_t s2o = (rp4lp1 & 0x7);
            DBG(cerr << "S1 -> " << s1o << " : S2 -> " << s2o << endl);

            //Find l4, l4*
            uint16_t l4 = (o & 0xFC0) >> 6;
            uint16_t l4s = (os & 0xFC0) >> 6;
            DBG(cerr << "L3 = " << l4 << " : L3* = " << l4s << endl);

            //Find E(l4), E(l4*)
            uint16_t el4 = expand(l4);
            uint16_t el4s = expand(l4s);
            DBG(cerr << "E(L3) = " << el4 << " : E(L3*) = " << el4s << endl);

            //Find XORs of inputs to S-Boxes
            uint8_t s1i = ((el4 ^ el4s) & 0xF0) >> 4;
            uint8_t s2i = (el4 ^ el4s) & 0xF;  
            DBG(cerr << "S1 <- " << s1i << " : S2 <- " << s2i << endl);            

            //Find candidate pairs that xor to the known input of s1
            //and the outputs from S boxes xor to the known output of s1
            set<uint8_t> s1candidates;
            for(const pair<uint8_t, uint8_t>& p : xor_lookup[s1i])
                if(S_BOXES[0][p.first] ^ S_BOXES[0][p.second] == s1o)
                {
                    s1candidates.insert(p.first ^ el4);
                    s1candidates.insert(p.second ^ el4);
                }

            set<uint8_t> s2candidates;
            for(const pair<uint8_t, uint8_t>& p : xor_lookup[s2i])
                if(S_BOXES[1][p.first] ^ S_BOXES[1][p.second] == s2o)
                {
                    s2candidates.insert(p.first ^ el4);
                    s2candidates.insert(p.second ^ el4);
                }

            vector<uint8_t> merge(16);

            auto range = set_intersection(s1candidates.begin(), s1candidates.end(), k1.begin(), k1.end(), merge.begin());
            k1 = vector<uint8_t>(merge.begin(), range);
            
            merge.clear();
            merge.resize(16);

            range = set_intersection(s2candidates.begin(), s2candidates.end(), k2.begin(), k2.end(), merge.begin());
            k2 = vector<uint8_t>(merge.begin(), range);
        }

        uint8_t kl = *(k1.begin());
        uint8_t kr = *(k2.begin());
        uint8_t k = (kl << 3) | (kr >> 1) | ((kr & 1) << 8);
        uint16_t block = dist(reng);

        if(des3(block) != encrypt(block, k))
            return k | (1 << 7);
        return k;
    }

    uint16_t crack4(function<uint16_t(uint16_t)> des4)
    {
        
    }
}