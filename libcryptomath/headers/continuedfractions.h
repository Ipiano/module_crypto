#ifndef CONTINUED_FRACTIONS_H
#define CONTINUED_FRACTIONS_H

#include <vector>
#include <utility>
#include <cstdint>

namespace cryptomath
{
namespace continuedfractions
{

std::vector<uint64_t> dtocf(double d, const uint64_t& max_iterations = 20);
std::vector<uint64_t> ftocf(uint64_t p, uint64_t q);
std::vector<uint64_t> sqrttocf(uint64_t n);

std::vector<std::pair<uint64_t, uint64_t>> cftof(const std::vector<uint64_t>& cf, uint64_t repetitions=0);
double cftod(const std::vector<uint64_t>& cf, uint64_t repetitions=0);

}
}
#endif