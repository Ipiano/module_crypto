#pragma once

#include <vector>
#include <functional>
#include <istream>
#include <cctype>

namespace frequency
{
    //Typedefs for function inputs
    template<class T>
    using incrementer = std::function<void(T&)>;

    template<class T>
    using mapper = std::function<uint64_t(const T&)>;

    //Core frequency counter
    //Uses mapper and incrementer to output data into
    //the output container
    template<class InIterator, class OutIterator, class I, class O>
    void countFrequencies(InIterator start, const InIterator& end, OutIterator out,
                          incrementer<O> incFn, mapper<I> mapFn)
    {
        while(start != end)
            incFn(*(out + mapFn(*(start++))));
    }

    //Convenience override
    //Frequency counter for a string of text
    template<class Iterator, class O>    
    void countFrequencies(const std::string& text, Iterator out, incrementer<O> incFn, bool caseSensitive = false)
    {
        countFrequencies<std::string::const_iterator, 
                         Iterator, char, O>(text.cbegin(), text.cend(), out, incFn,
                                           [=](const char& v){
                                               return caseSensitive ? (uint8_t)v : tolower((uint8_t)v);
                                           });
    }

    //Convenience override
    //Frequency counter for an ifstream
    template<class Iterator, class O>    
    void countFrequencies(std::istream& input, Iterator out, incrementer<O> incFn, bool caseSensitive = false)
    {
        std::string line;
        while(input >> line)
            countFrequencies(line, out, incFn, caseSensitive);
    }

}