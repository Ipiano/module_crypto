#pragma once

#include <vector>
#include <functional>
#include <istream>
#include <cctype>

namespace frequency
{
    template<class InIterator, class OutIterator, class T>
    void countFrequencies(InIterator start, const InIterator& end, OutIterator out,
                          std::function<uint64_t(const T&)> mapFun = [](const T& v){return (uint64_t)v;})
    {
        while(start != end)
            (*(out + mapFun(*(start++))))++;
    }

    template<class Iterator>    
    void countFrequencies(const std::string& text, Iterator out, bool caseSensitive = false)
    {
        countFrequencies<std::string::const_iterator, 
                         Iterator, char>(text.cbegin(), text.cend(), out,
                                        [=](const char& v){
                                            return caseSensitive ? (uint8_t)v : tolower((uint8_t)v);
                                        });
    }

    template<class Iterator>    
    void countFrequencies(std::istream& input, Iterator out, bool caseSensitive = false)
    {
        std::string line;
        while(input >> line)
            countFrequencies(line, out, caseSensitive);
    }

}