/*! @file */ 
#pragma once

#include <vector>
#include <functional>
#include <istream>
#include <cctype>

/*! Contains functions for counting the number of occurances of things in sequences */
namespace frequency
{
    /*! Function that increments a type T */
    template<class T>
    using incrementer = std::function<void(T&)>;

    /*! Function that maps a type T to an unsigned 64-bit int */
    template<class T>
    using mapper = std::function<uint64_t(const T&)>;

    /*! \brief Core frequency counter

    Processes all data provided between the start and end input iterators. Each
    value is mapped to a value in the output sequence using the map function. The
    data is accessed by adding the index to the output iterator, and that value
    is incremented with the increment function.
      
    Template arguments
        - class InIterator - Iterator type for sequence to process
        - class OutIterator - Iterator type for sequence to store results in
        - class I - Type of data read from input sequence
        - class O - Type of data to increment in output sequence

    \param[in] start Beginning of range of things to count
    \param[in] end 1 after the end of the range of things to count
    \param[in,out] out Beginning of range to output to
    \param[in] incFn Function to increment type O
    \param[in] mapFn Function to map type I to a location in the range starting at out
    */
    template<class InIterator, class OutIterator, class I, class O>
    void countFrequencies(InIterator start, const InIterator& end, OutIterator out,
                          incrementer<O> incFn, mapper<I> mapFn)
    {
        while(start != end)
            incFn(*(out + mapFn(*(start++))));
    }

    /*! \brief Convenience function to count frequency in a string

    Processes all data provided between the start and end input iterators. The output iterator's sequence
    should have a size of at least 255. Characters are mapped to the range 0-255 by their ASCII value.
    If the function is not case sensitive, then upper-case values are mapped to their lower-case equivalent's
    ASCII value. The data is accessed by adding the index to the output iterator, and that value
    is incremented with the increment function.
      
    Template arguments
        - class Iterator - Iterator type for sequence to store results in
        - class O - Type of data to increment in output sequence

    \param[in] text String of data to frequency count
    \param[in] out Beginning of range to output to
    \param[in] incFn Function to increment type O
    \param[in] caseSensitive If true, capital letters are counted separate from their lower-case counterparts. Default false
    */
    template<class Iterator, class O>    
    void countFrequencies(const std::string& text, Iterator out, incrementer<O> incFn, bool caseSensitive = false)
    {
        countFrequencies<std::string::const_iterator, 
                         Iterator, char, O>(text.cbegin(), text.cend(), out, incFn,
                                           [=](const char& v){
                                               return caseSensitive ? (uint8_t)v : tolower((uint8_t)v);
                                           });
    }

    /*! \brief Convenience function to count frequency in a text stream

    Processes all data provided by the input stream. Data is read using std::getline. The output iterator's sequence
    should have a size of at least 255. Characters are mapped to the range 0-255 by their ASCII value.
    If the function is not case sensitive, then upper-case values are mapped to their lower-case equivalent's
    ASCII value. The data is accessed by adding the index to the output iterator, and that value
    is incremented with the increment function.
      
    Template arguments
        - class Iterator - Iterator type for sequence to store results in
        - class O - Type of data to increment in output sequence

    \param[in] input Input stream containing text to count frequency in
    \param[in] out Beginning of range to output to
    \param[in] incFn Function to increment type O
    \param[in] caseSensitive If true, capital letters are counted separate from their lower-case counterparts. Default false
    */
    template<class Iterator, class O>    
    void countFrequencies(std::istream& input, Iterator out, incrementer<O> incFn, bool caseSensitive = false)
    {
        std::string line;
        while(std::getline(input, line))
            countFrequencies(line, out, incFn, caseSensitive);
    }

}