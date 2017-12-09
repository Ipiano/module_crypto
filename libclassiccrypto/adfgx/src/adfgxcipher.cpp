/*!
\file
*/
#include "../headers/adfgxcipher.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

#ifndef DEBUG
    #define DBG(a) 
#else
    #include <iostream>
    #include <iomanip>
    #define DBG(a) a
#endif

namespace adfgx
{
    const std::unordered_map<char, std::unordered_map<char, char>> transformer::DECRYPT_TABLE
    {
        {'a', {{'a', 'p'}, {'d', 'g'}, {'f', 'c'}, {'g', 'e'}, {'x', 'n'}}},
        {'d', {{'a', 'b'}, {'d', 'q'}, {'f', 'o'}, {'g', 'z'}, {'x', 'r'}}},
        {'f', {{'a', 's'}, {'d', 'l'}, {'f', 'a'}, {'g', 'f'}, {'x', 't'}}},
        {'g', {{'a', 'm'}, {'d', 'd'}, {'f', 'v'}, {'g', 'i'}, {'x', 'w'}}},
        {'x', {{'a', 'k'}, {'d', 'u'}, {'f', 'y'}, {'g', 'x'}, {'x', 'h'}}}            
    };

    const std::unordered_map<char, std::string> transformer::ENCRYPT_TABLE  
    {
        {'p',"aa"},
        {'g',"ad"},
        {'c',"af"},
        {'e',"ag"},
        {'n',"ax"},
        {'b',"da"},
        {'q',"dd"},
        {'o',"df"},
        {'z',"dg"},
        {'r',"dx"},
        {'s',"fa"},
        {'l',"fd"},
        {'a',"ff"},
        {'f',"fg"},
        {'t',"fx"},
        {'m',"ga"},
        {'d',"gd"},
        {'v',"gf"},
        {'i',"gg"},
        {'w',"gx"},
        {'k',"xa"},
        {'u',"xd"},
        {'y',"xf"},
        {'x',"xg"},
        {'h',"xx"}
    };

    //! Represents a column of letters under the key
    struct column
    {
        //! The letters in the column, from top to bottom
        string col;

        //! The letter of the key the letters are under
        char key;
    };

    //! < operator for the column type; allows sorting of the columns
    bool operator < (const column& l, const column& r)
    {
        return l.key < r.key;
    }

    /*! Constructs an ADFGX transformer with the given key.
    The key must 
        - Be non-empty
        - Contain no duplicates

        \param[in] key The key to use for the cipher
        \throws logic_error : The key is empty
        \throws logic_error : The key has duplicate letters
    */
    transformer::transformer(const std::string& key) : _key(key)
    {
        if(!_key.size())
            throw std::logic_error("key must be non-empty");

        for(const char& c : _key)
            if(_key.find(c) != _key.rfind(c))
                throw std::logic_error("key must not contain duplicate letters");
    }

    /*! Encrypts a string using the ADFGX cipher method and the
    default substituion matrix. Any characters of the message which are
    not part of the english alphabet will be removed

    \param[in] message The string to encrypt
    \returns string - The resulting ciphertext
    */
    std::string transformer::encrypt(const std::string& message)
    {
        string cipher;
        string lowerMsg(message.size(), ' '); 

        //Make all characters lowercase
        transform(message.begin(), message.end(), lowerMsg.begin(), [](unsigned char c){ return std::tolower(c);});

        DBG(cerr << message << " -> " << lowerMsg << endl);

        //Substitute pair from [adfgx] for each character
        for(const char& c : lowerMsg)
        {
            auto encrypted = ENCRYPT_TABLE.find(c);
            if(encrypted != ENCRYPT_TABLE.end())
                cipher += encrypted->second;
        }

        DBG(cerr << lowerMsg << " -> " << cipher << endl);
        
        vector<column> cols(_key.size());

        //Write columns under key
        for(int i=0; i<_key.size(); i++)
        {
            cols[i].key = _key[i];
            for(int j=i; j < cipher.size(); j+=_key.size())
                cols[i].col.push_back(cipher[j]);
        }

        DBG(cerr << cipher << " -> " << endl;
        for(const column& c : cols)
            cerr << c.key << " : " << c.col << endl;)

        //Sort columns so key letters are alphabetic
        sort(cols.begin(), cols.end());
        cipher.clear();

        DBG(cerr << " -> " << endl;
            for(const column& c : cols)
                cerr << c.key << " : " << c.col << endl;)
        
        //Concatentate columns to form ciphertext
        for(const column& c : cols)
            cipher += c.col;

        DBG(cerr << " -> " << cipher << endl);

        return cipher;
    }

    /*! Decrypts an ADFGX ciphertext using the default substitution matrix.
    All letters decrypted will be lower-case values in the range a-z. Any characters
    in the ciphertext which are not from [adfgx] will be removed.

    \param[in] cipher The text to decrypt
    \returns string - The decrypted message
    */
    std::string transformer::decrypt(const std::string& cipher)
    {
        string lowerCiph;
        string message;

        //Strip all invalid characters
        for(const char& c : cipher)
        {
            char c_ = tolower((unsigned char)c);
            if(DECRYPT_TABLE.count(c_))
                lowerCiph.push_back(c_);
        }

        DBG(cerr << cipher << " -> " << lowerCiph << endl);

        //Determine column size and number of long columns
        uint32_t rows = lowerCiph.size() / _key.size();
        uint32_t extras = lowerCiph.size() % _key.size();

        //Initialize columns in alphabetic key order
        vector<column> cols(_key.size());
        for(int i = 0; i < _key.size(); i++) cols[i].key = _key[i];
        sort(cols.begin(), cols.end());

        //Fill columns
        int ind = 0;
        for(column& c : cols)
        {
            uint32_t len = rows + (_key.find(c.key) < extras ? 1 : 0);
            c.col = lowerCiph.substr(ind, len);
            ind += len;
        }

        DBG(cerr << lowerCiph << " -> " << endl;
        for(const column& c : cols)
            cerr << c.key << " : " << c.col << endl;)

        //'Sort' columns back into original key order
        sort(cols.begin(), cols.end(), 
            [this](const column& l, const column& r)
            {
                return _key.find(l.key) < _key.find(r.key);
            });

        DBG(cerr << " -> " << endl;
            for(const column& c : cols)
                cerr << c.key << " : " << c.col << endl;)

        lowerCiph.clear();

        //Concatenate rows into substituted text
        bool done = false;
        int r = 0;
        while(!done)
        {
            for(const column& c : cols)
            {
                if(c.col.size() > r)
                {
                    lowerCiph.push_back(c.col.at(r));
                }
                else
                {
                    done = true;
                    break;
                }
            }
            r++;
        }

        DBG(cerr << " -> " << lowerCiph << endl);

        //Undo substitution using default matrix
        for(int i=0; i<lowerCiph.size(); i+=2)
        {
            message.push_back(DECRYPT_TABLE.at(lowerCiph[i]).at(lowerCiph[i+1]));
        }

        DBG(cerr << lowerCiph << " -> " << message << endl;)

        return message;
    }
    
}