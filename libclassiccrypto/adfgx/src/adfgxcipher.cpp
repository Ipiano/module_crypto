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

    struct column
    {
        string col;
        char key;
    };

    bool operator < (const column& l, const column& r)
    {
        return l.key < r.key;
    }

    transformer::transformer(const std::string& key) : _key(key)
    {
        if(!_key.size())
            throw std::logic_error("key must be non-empty");

        for(const char& c : _key)
            if(_key.find(c) != _key.rfind(c))
                throw std::logic_error("key must not contain duplicate letters");
    }

    std::string transformer::encrypt(const std::string& message)
    {
        string cipher;
        string lowerMsg(message.size(), ' '); 
        transform(message.begin(), message.end(), lowerMsg.begin(), [](unsigned char c){ return std::tolower(c);});

        DBG(cerr << message << " -> " << lowerMsg << endl);

        for(const char& c : lowerMsg)
        {
            auto encrypted = ENCRYPT_TABLE.find(c);
            if(encrypted != ENCRYPT_TABLE.end())
                cipher += encrypted->second;
        }

        DBG(cerr << lowerMsg << " -> " << cipher << endl);
        
        vector<column> cols(_key.size());

        for(int i=0; i<_key.size(); i++)
        {
            cols[i].key = _key[i];
            for(int j=i; j < cipher.size(); j+=_key.size())
                cols[i].col.push_back(cipher[j]);
        }

        DBG(cerr << cipher << " -> " << endl;
        for(const column& c : cols)
            cerr << c.key << " : " << c.col << endl;)

        sort(cols.begin(), cols.end());
        cipher.clear();

        DBG(cerr << " -> " << endl;
            for(const column& c : cols)
                cerr << c.key << " : " << c.col << endl;)
        
        for(const column& c : cols)
            cipher += c.col;

        DBG(cerr << " -> " << cipher << endl);

        return cipher;
    }

    std::string transformer::decrypt(const std::string& cipher)
    {
        string lowerCiph;
        string message;
        for(const char& c : cipher)
        {
            char c_ = tolower((unsigned char)c);
            if(DECRYPT_TABLE.count(c_))
                lowerCiph.push_back(c_);
        }

        DBG(cerr << cipher << " -> " << lowerCiph << endl);

        uint32_t rows = lowerCiph.size() / _key.size();
        uint32_t extras = lowerCiph.size() % _key.size();

        vector<column> cols(_key.size());
        for(int i = 0; i < _key.size(); i++) cols[i].key = _key[i];
        sort(cols.begin(), cols.end());

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

        sort(cols.begin(), cols.end(), 
            [this](const column& l, const column& r)
            {
                return _key.find(l.key) < _key.find(r.key);
            });

        DBG(cerr << " -> " << endl;
            for(const column& c : cols)
                cerr << c.key << " : " << c.col << endl;)

        lowerCiph.clear();
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
        for(int i=0; i<lowerCiph.size(); i+=2)
        {
            message.push_back(DECRYPT_TABLE.at(lowerCiph[i]).at(lowerCiph[i+1]));
        }

        DBG(cerr << lowerCiph << " -> " << message << endl;)

        return message;
    }
    
}