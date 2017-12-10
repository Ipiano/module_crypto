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
        /*!
            The expansion function is defined for a 6-bit number 0 1 2 3 4 5 as 0 1 3 2 3 2 4 5

            \param[in] n The 6 bit number to expand
            \returns uint8_t - The expanded value
        */
        uint8_t expand(const uint8_t& n)
        { 
            //          1, 2              3 - Stay           3 - Swap
            return ((n & 0x30) << 2) | ((n & 0x8) << 1) | ((n & 0x8) >> 1) |
            //        4 - Stay              4 - Swap         5, 6
                    ((n & 0x4) << 1) | ((n & 0x4) << 3) | (n & 0x3);
        }

        /*!
            A round key \f$ k_i \f$ is defined as the initial key \f$ k \f$ <<< \f$ i\f$ , where <<< is the rotate left operation.
            This function rotates the 9-bit key left once, and returns the lower 8 bits. For this to work correctly to obtain
            \f$ k_0 \f$, the original key must be rotated to the right once at the beginning of encryption.

            \param[in,out] key The original key, which has been rotated left \f$ i-1 \f$ times
            \returns uint8_t - \f$ k_i \f$
        */
        uint8_t next_key(uint16_t& key)
        {
            //Rotate left so next key is bottom 8 bits
            key = (key << 1) | ((key & 0x1FF) >> 8);

            //Extract bottom 8 bits
            return 0xFF & key;
        }

        /*!
            A round key \f$ k_i \f$ is defined as the initial key \f$ k \f$ <<< \f$ i\f$ , where <<< is the rotate left operation.
            This function rotates the 9-bit key right once, and returns the lower 8 bits. For this to work correctly to obtain
            \f$ k_n \f$, the original key must be rotated to the left \f$ n+1 \f$ at the beginning of decryption. (\f$ n \f$ is the number of rounds)

            \param[in,out] key The original key, which has been rotated right \f$ i-1 \f$ times
            \returns uint8_t - \f$ k_i \f$
        */
        uint8_t prev_key(uint16_t& key)
        {
            //Rotate right so next key is bottom 8 bits
            key = (key >> 1) | (key << 8);

            //Extract bottom 8 bits
            return 0xFF & key;
        }

        /*!
            The F function for the simplified DES is very similar to the F function for the full DES. It takes 6 bits, \f$ r_i \f$, the right
            half of a block, and a round key \f$ k_i \f$.
                -# Expand \f$ r_i \f$
                -# XOR \f$ k_i \f$ with the expanded \f$ r_i \f$
                -# Split the result into left and right 4 bits
                -# Apply S1 to the left, and S2 to the right
                -# Concatenate the outputs of the S-Boxes
            
            \param[in] r The right half of the current block
            \param[in] key Round key \f$ k_i \f$
            \returns uint8_t - The result of the F function
        */
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

        /*! 
            Since rounds are the same for both encryption and decryption (just with different key orders), this
            function is used for both. Given a block of the form \f$ l_ir_i \f$ and a round key \f$ k_i \f$ a round is as follows:
                -# \f$ l_{i+1} = r_i \f$
                -# \f$ r_{i+1} = F(r_i) \mathbin{\oplus} l_i \f$

            \param[in] block The block to run the round on
            \param[in] key The round key \f$ k_i \f$
            \returns uint16_t - The new block
        */
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

        /*!
            If two encrypted blocks \f$ L_3R_3, L^*_3R^*_3 \f$ are known, and 
            \f$ R_3'L_0' \f$ are known with \f$ R_i' = R_i \mathbin{\oplus} R_i^*, L_i' = L_i \mathbin{\oplus} L_i^* \f$,
            then if \f$ R_0 = R_0^* \f$, the simplified DES equations can be rearranged in such a way that
            possible halves of \f$ k_3 \f$ can be guessed.
            \f[R_1 = L_0 \mathbin{\oplus} f(R_0, K_1)\f]
            \f[L_2 = R_1 = L_0 \mathbin{\oplus} f(R_0, K_1)\f]
            \f[R_3 = L_2 \mathbin{\oplus} F(R_2, K_3) = L_0 \mathbin{\oplus} F(R_0, K_1) \mathbin{\oplus} F(R_2, K_3) \f]

            Because we are assuming that \f$ R_0 = R_0^* \f$, we know that \f$ F(R_0, K_i) = F(R_0^*, K_i) \f$ and we can write
            \f[R_3' = R_3 \mathbin{\oplus} R_3^* = L_0' \mathbin{\oplus} F(R_2, K_3) \mathbin{\oplus} F(R_2^*, K_3) \f] 
            \f[R_3' \mathbin{\oplus} L_0' = F(R_2, K_3) \mathbin{\oplus} F(R_2^*, K_3) \f] 
            
            Since \f$R_2 = L_3, R_2* = L_3*\f$
            \f[R_3' \mathbin{\oplus} L_0' = F(L_3, K_3) \mathbin{\oplus} F(L_3^*, K_3) \f]

            So, since we know the two input blocks used to get \f$ L_3R_3, L^*_3R^*_3 \f$, everything in this equation except for \f$ K_3 \f$ is known.
            The insights needed to use this equation involve looking at how the S-Boxes are used to calculate \f$ F(L_3, K_3), F(L_3^*, K_3) \f$.
            We know the bits sent into the S-Boxes in these cases are the left and right halves of \f$ E(L_3) \mathbin{\oplus} K_3 \f$ and \f$ E(L_3^*) \mathbin{\oplus} K_3 \f$
            respectively. Its fairly obvious that, because of how the expansion function works, we have 
            \f[E(L_3) \mathbin{\oplus} E(L_3^*) = E(L_3 \mathbin{\oplus} L_3^*) = E(L_3') \f]
            We also know the left and right 3-bit values outputted by the S-Boxes: \f$ R_3' \mathbin{\oplus} L_0' \f$.
            Using these two pieces of information, we can find all possible 4-bit pairs that could be the left side
            of \f$ E(L_3') \f$ and all 4-bit pairs that could be the right side.

            \param[in] rp3lp0 \f$ R_3' \mathbin{\oplus} L_0' \f$
            \param[in] o \f$ L_3R_3 \f$
            \param[in] os \f$ L_3^*R_3^* \f$
            \param[out] s1candidates Possible left halves of \f$ k_3 \f$
            \param[out] s2candidates Possible right halves of \f$ k_3 \f$
        */
        void analyze3(const uint16_t& rp3lp0, const uint16_t& o, const uint16_t& os,
                      std::set<uint8_t>& s1candidates, std::set<uint8_t>& s2candidates)
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

    /*!
        Since the next_key function does the rotate before returning \f$ k_i \f$, the key
        inputted to this function is first rotated to key \f$ k_{-1} \f$. The simplified
        DES round function is run the specified number of times, and the result is returned

        \param[in] block The 12-bit block to encrypt
        \param[in] key The 9-bit key to use
        \param[in] rounds The number of rounds to do
        \returns uint16_t - The encrypted 12-bit block
    */
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

    /*!
    Since the prev_key function does the rotate before returning \f$ k_i \f$, the key
    inputted to this function is first rotated to key \f$ k_{n+1} \f$. The simplified
    DES round function is run the specified number of times, and the result is returned

    \param[in] block The 12-bit block to decrypt
    \param[in] key The 9-bit key to use
    \param[in] rounds The number of rounds to do
    \returns uint16_t - The decrypted 12-bit block
    */
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

    /*!
        Using the analyze3 function of the des4::_internal namespace, we can
        obtain the key for a 3 round simplified DES by trying multiple inputs
        and seeing what they encrypt to.

        Since the analyze3 function returns multiple possibilities for the left and
        right halves of the final key, it is run multiple times with different inputs
        until only 1 possible left and right half remain.

        Since this still leaves 1 bit of the full 9-bit key unknown, the key is tested
        with both 0 and 1 for that unknown bit to see which encrypts to the correct value.

        \param[in] des3 A function which encrypts using 3 rounds of the simplified DES and some unknown key
        \returns uint16_t - The 9-bit key that the encryption function is using
        \throws logic_error : No options are left for the left or right half of the key
        \throws logic_error : Neither version of the 9-bit key encrypts to the expected value
    */
    uint16_t crack3(std::function<uint16_t(uint16_t)> des3)
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

            //Analyze to get left and right key candidates
            set<uint8_t> k1candidates, k2candidates;
            analyze3(rp3lp0, o, os, k1candidates, k2candidates);

            vector<uint8_t> merge(16);

            //Merge candidate key halves with existing list to get the list of halves that 
            //have be the result every time
            auto range = set_intersection(k1candidates.begin(), k1candidates.end(), k1.begin(), k1.end(), merge.begin());
            k1 = vector<uint8_t>(merge.begin(), range);
            
            merge.clear();
            merge.resize(16);

            range = set_intersection(k2candidates.begin(), k2candidates.end(), k2.begin(), k2.end(), merge.begin());
            k2 = vector<uint8_t>(merge.begin(), range);
        }

        //Build the initial key from the halves
        uint8_t kl = *(k1.begin());
        uint8_t kr = *(k2.begin());
        uint16_t k = (kl << 3) | (kr >> 1) | ((kr & 1) << 8);

        DBG(cerr << "Key parts = " << bin(kl) << " : " << bin(kr) << endl);
        DBG(cerr << "Key is maybe " << bin(k) << endl);

        //Pick a random block and encrypt it
        //Check if both encryptions match
        //If so, we know the key, if not, flip the unknown bit and try again
        uint16_t block = dist(reng);
        uint16_t match = des3(block);
        if(match == encrypt(block, k, 3))
            return k;

        DBG(cerr << "Nope, key is " << bin((uint16_t)(k | (1 << 7))) << endl);
        k |= (1 << 7);
        
        if(match == encrypt(block, k, 3))
            return k;
        
        throw std::logic_error("unable to crack");
    }

    /*!
        An analysis of the S-Boxes of the simplified DES shows that not all outputs are equally likely.
            - For Box 1, if two 4-bit values XOR to 0011 there is a 3/4 chance that the XOR of their outputs is 011
            - For Box 2, if two 4-bit values XOR to 1100 there is a 1/2 chance that the XOR of their outputs is 010

        Therefore, if the S-Boxes are independent, there is a 3/8 chance that, for any given \f$ R_i, R_i^* \f$ which XOR to 00111100, the output
        of the outputs of \f$ F(R_i), F(R_i^*) \f$ will XOR to 011010. (They're not actually independent, some of the bits of \f$ R_i \f$ get
        used in both because of how the expansion function works, but it's close enough)

        Using this, we can pick some \f$L_0, L_0^*\f$ such that \f$L_0 \mathbin{\oplus} L_0^* = L_0' = 011010\f$. Since
        \f$ L_0 \f$ is XORed with the output of the F function, we now have a 3/8 chance that \f$ R_1' = 000000 \f$. If this is the case
        it also follows that \f$ R_1 = R_1^* \f$

        So, using the above insights, we can say that, if we pick \f$ R_0, R_0^* \f$ such that \f$ R_0' = 001100\f$
        and some \f$ L_0, L_0^*\f$ such that \f$ L_0' = 011010 \f$, there is a 3/8 chance that \f$ L_1'R_1' = 001100000000\f$.

        At this point, if we know the output after four rounds of the simplified DES, we can use the 3 round differential
        analysis to get potential key halfs for \f$ k_4 \f$. If the input values \f$ L_0R_0, L_0^*R_0^*\f$ are randomly chosen, then
        3/8 of the time, the assumption that \f$ L_1'R_1' = 001100000000\f$ will be correct, and the actual key halves will be returned.
        The rest of the time, incorrect keys will be returned; but there is no particular reason that any particular wrong key part
        should be more likely than another. Therefore, if this process is done Many Times\f$^{TM}\f$, the most commonly returned key
        halves are likely the correct halves of \f$ k_f \f$. The final bit of the original key can be found by trying both 0 and 1

        \param[in] des4 A function which encrypts using 3 rounds of the simplified DES and some unknown key
        \param[in] iterations The number of times to repeat the above process to get potential key halves
        \returns uint16_t - The 9-bit key that the encryption function is using
        \throws logic_error : Neither version of the 9-bit key encrypts to the expected value
    */
    uint16_t crack4(std::function<uint16_t(uint16_t)> des4, uint64_t iterations)
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

        //Find the most frequent left half
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

        //Find the most frequent right half
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

        //Build the key from the most frequent key halves
        uint16_t k = (kl << 2) | (kr >> 2) | ((kr & 3) << 7);
        
        DBG(cerr << "Key parts = " << bin(kl) << " : " << bin(kr) << endl);
        DBG(cerr << "Key is maybe " << bin(k) << endl);

        //Pick a random block and encrypt it
        //Check if both encryptions match
        //If so, we know the key, if not, flip the unknown bit and try again
        uint16_t block = dist(reng);        
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