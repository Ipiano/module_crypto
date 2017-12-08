#include "../headers/des4.h"

#include <random>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <chrono>

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

        void analyze3(const uint16_t& rp3lp0, const uint16_t& o, const uint16_t& os,
                      set<uint8_t>& s1candidates, set<uint8_t>& s2candidates)
        {
            s1candidates.clear();
            s2candidates.clear();

            //Find output XORs from S-Boxes
            uint8_t s1o = (rp3lp0 & 0x38) >> 3;
            uint8_t s2o = (rp3lp0 & 0x7);
            DBG(cerr << "S1 -> " << bin(s1o) << " : S2 -> " << bin(s2o) << endl);

            //Find l3, l3*
            uint16_t l3 = (o & 0xFC0) >> 6;
            uint16_t l3s = (os & 0xFC0) >> 6;
            DBG(cerr << "L3 = " << bin(l3) << " : L3* = " << bin(l3s) << endl);

            //Find E(l3), E(l3*)
            uint16_t el3 = expand(l3);
            uint16_t el3s = expand(l3s);
            DBG(cerr << "E(L3) = " << bin(el3) << " : E(L3*) = " << bin(el3s) << endl);

            //Find XORs of inputs to S-Boxes
            uint8_t s1i = ((el3 ^ el3s) & 0xF0) >> 4;
            uint8_t s2i = (el3 ^ el3s) & 0xF;  
            DBG(cerr << "S1 <- " << bin(s1i) << " : S2 <- " << bin(s2i) << endl);            

            //Find candidate pairs that xor to the known input of s1
            //and the outputs from S boxes xor to the known output of s1
            for(const pair<uint8_t, uint8_t>& p : XOR_LOOKUP[s1i])
                if((S_BOXES[0][p.first] ^ S_BOXES[0][p.second]) == s1o)
                {
                    DBG(cerr << "Left candidate: " << bin(p.first) << ", " << bin(p.second) << " -> ");
                    DBG(cerr << bin((uint8_t)(p.first ^ ((el3 & 0xF0) >> 4))) << ", " << bin((uint8_t)(p.second ^ ((el3 & 0xF0) >> 4))) << endl);
                    s1candidates.insert(p.first ^ ((el3 & 0xF0) >> 4));
                    s1candidates.insert(p.second ^ ((el3 & 0xF0) >> 4));
                }

            for(const pair<uint8_t, uint8_t>& p : XOR_LOOKUP[s2i])
                if((S_BOXES[1][p.first] ^ S_BOXES[1][p.second]) == s2o)
                {
                    DBG(cerr << "Right candidate: " << bin(p.first) << ", " << bin(p.second) << " -> ");
                    DBG(cerr << bin((uint8_t)(p.first ^ (el3 & 0xF))) << ", " << bin((uint8_t)(p.second ^ (el3 & 0xF))) << endl);
                    s2candidates.insert(p.first ^ (el3 & 0xF));
                    s2candidates.insert(p.second ^ (el3 & 0xF));
                }
        }
    }

    uint16_t encrypt(uint16_t block, uint16_t key, const uint64_t& rounds)
    {
        using namespace _internal;

        DBG(cerr << "------------DES4-----------" << endl);
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
        DBG(cerr << "---------------------------" << endl);
        
        return block & 0xFFF;
    }

    uint16_t decrypt(uint16_t block, uint16_t key, const uint64_t& rounds)
    {
        using namespace _internal;

        DBG(cerr << "------------DES4-----------" << endl);        
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
        DBG(cerr << "---------------------------" << endl);
        
        return block & 0xFFF;
    }

    uint16_t crack3(function<uint16_t(uint16_t)> des3)
    {
        using namespace _internal;

        DBG(cerr << "Cracking DES3" << endl);

        //Random generator to get test cases
        auto t = std::chrono::system_clock::now();
        mt19937 reng(chrono::duration_cast<chrono::milliseconds>(t.time_since_epoch()).count());
        uniform_int_distribution<> dist(0, 0xFFF);

        //Candidate sets or left and right parts of k
        //Repeat until these both have 1 item left
        vector<uint8_t> k1 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
        vector<uint8_t> k2 = k1;

        //int z = 0;
        while(k1.size() != 1 || k2.size() != 1)
        {
            DBG(cerr << k1.size() << " potential key lefts" << endl);
            DBG(cerr << k2.size() << " potential key rights" << endl);

            //Should never happen
            if(k1.size() == 0 || k2.size() == 0)
                throw logic_error("key left or key right has 0 options");

            //Pick two random inputs with R1 = R1*
            uint16_t i = dist(reng);
            uint16_t is = (dist(reng) & 0xFC0) | (i & 0x3F);

            //Test cases from book
            //--------------
            //z++;
            //uint16_t i =  (z == 1 ? 0b000111011011 : 0b010111011011);
            //uint16_t is = (z == 1 ? 0b101110011011 : 0b101110011011);
            //--------------
            DBG(cerr << "LR1 = " << bin(i) << " : LR1* = " << bin(is) << endl);

            //Get their outputs
            uint16_t o = des3(i);
            uint16_t os = des3(is);
            DBG(cerr << "LR3 = " << bin(o) << " : LR3* = " << bin(os) << endl);

            //Compute r3' ^ l1'
            uint16_t rp3lp0 = (((i ^ is) & 0xFC0) >> 6) ^ ((o ^ os) & 0x3F);
            DBG(cerr << "R3\' ^ L1\' = " << bin(rp3lp0) << endl);

            set<uint8_t> k1candidates, k2candidates;
            analyze3(rp3lp0, o, os, k1candidates, k2candidates);

            vector<uint8_t> merge(16);

            auto range = set_intersection(k1candidates.begin(), k1candidates.end(), k1.begin(), k1.end(), merge.begin());
            k1 = vector<uint8_t>(merge.begin(), range);
            
            merge.clear();
            merge.resize(16);

            range = set_intersection(k2candidates.begin(), k2candidates.end(), k2.begin(), k2.end(), merge.begin());
            k2 = vector<uint8_t>(merge.begin(), range);
        }

        uint8_t kl = *(k1.begin());
        uint8_t kr = *(k2.begin());
        uint16_t k = (kl << 3) | (kr >> 1) | ((kr & 1) << 8);
        uint16_t block = dist(reng);

        DBG(cerr << "Key parts = " << bin(kl) << " : " << bin(kr) << endl);
        DBG(cerr << "Key is maybe " << bin(k) << endl);

        uint16_t match = des3(block);
        if(match == encrypt(block, k, 3))
            return k;

        DBG(cerr << "Nope, key is " << bin((uint16_t)(k | (1 << 7))) << endl);
        k |= (1 << 7);
        
        if(match == encrypt(block, k, 3))
            return k;
        
        throw std::logic_error("unable to crack");
}

    uint16_t crack4(function<uint16_t(uint16_t)> des4, uint64_t iterations)
    {
        using namespace _internal;

        //Random generator to get test cases
        auto t = std::chrono::system_clock::now();
        mt19937 reng(chrono::duration_cast<chrono::milliseconds>(t.time_since_epoch()).count());
        uniform_int_distribution<> dist(0, 0xFFF);

        unordered_map<uint8_t, uint64_t> left_freqs;
        unordered_map<uint8_t, uint64_t> right_freqs;

        for(int z = 0; z < iterations; z++)
        {
            //Pick random r0l0
            uint16_t i = dist(reng);

            //Find r0*l0* with specific xor
            uint16_t is = (i ^ 0b011010001100);

            //Run des4
            uint16_t o = des4(i);
            uint16_t os = des4(is);

            uint16_t rp3lp0 = 0xC ^ ((o ^ os) & 0x3F);

            //Assume l1'r1' = 001100000000
            //And run 3 round analysis to get statistically
            //likely portions of key
            set<uint8_t> k1candids, k2candids;
            analyze3(rp3lp0, o, os, k1candids, k2candids);

            for(uint8_t k : k1candids)
                left_freqs[k]++;

            for(uint8_t k : k2candids)
                right_freqs[k]++;
            
        }

        uint8_t kl = left_freqs.begin()->first;
        uint64_t left_count = left_freqs.begin()->second;
        
        uint8_t kr = right_freqs.begin()->first;
        uint8_t right_count = right_freqs.begin()->second;

        DBG(cerr << "Left frequencies: " << endl);
        for(auto iter = left_freqs.begin(); iter != left_freqs.end(); iter++)
        {
            DBG(cerr << bin(iter->first) << " : " << iter->second << endl);
            if(iter->second > left_count)
            {
                left_count = iter->second;
                kl = iter->first;
            }
        }

        DBG(cerr << "Right frequencies: " << endl);        
        for(auto iter = right_freqs.begin(); iter != right_freqs.end(); iter++)
        {
            DBG(cerr << bin(iter->first) << " : " << iter->second << endl);            
            if(iter->second > right_count)
            {
                right_count = iter->second;
                kr = iter->first;
            }
        }

        uint16_t k = (kl << 2) | (kr >> 2) | ((kr & 3) << 7);
        uint16_t block = dist(reng);
        
        DBG(cerr << "Key parts = " << bin(kl) << " : " << bin(kr) << endl);
        DBG(cerr << "Key is maybe " << bin(k) << endl);

        uint16_t match = des4(block);
        if(match == encrypt(block, k, 4))
            return k;

        DBG(cerr << "Nope, key is " << bin((uint16_t)(k | (1 << 6))) << endl);
        k |= (1 << 6);
        if(match == encrypt(block, k, 4))
            return k;
        
        throw std::logic_error("unable to crack");
    }
}