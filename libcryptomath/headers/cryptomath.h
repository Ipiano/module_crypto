/*! \file */ 
#pragma once

#include <iostream>

#ifndef DEBUG
    #define DBGOUT(a) 
#else
    #include <iostream>
    #define DBGOUT(a) std::cerr << a << std::endl;
#endif

/*! Contains a variety of functions used in cryptography and modular arithmetic.
As well as some template specializations to make most of the functions compatible
with the GNU Multiprecision library (GMP)
*/
namespace cryptomath{}

#include "./math_factoring.h"
#include "./math_misc.h"
#include "./math_modulararith.h"
#include "./math_primality.h"

#ifdef CRYPTOMATH_GMP
#include "./specializations_gmp.h"
#endif