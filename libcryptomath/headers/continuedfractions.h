/*! \file */ 
#pragma once

#include <vector>
#include <utility>
#include <cstdint>

namespace cryptomath
{
/*! Contains functions for converting to and from continued fractions */
namespace continuedfractions
{

/*! \brief Converts a real value to a continued fraction */
std::vector<uint64_t> dtocf(double d, const uint64_t& max_iterations = 20);

/*! \brief Converts a rational to a continued fraction */
std::vector<uint64_t> ftocf(uint64_t p, uint64_t q);

/*! \brief Converts a simple square root to a repeating continued fraction */
std::vector<uint64_t> sqrttocf(uint64_t n);

/*! \brief Converts a continued fraction to approximate rationals */
std::vector<std::pair<uint64_t, uint64_t>> cftof(const std::vector<uint64_t>& cf, uint64_t repetitions=0);

/*! \brief Converts a continued fraction to a real value */
double cftod(const std::vector<uint64_t>& cf, uint64_t repetitions=0);

}
}