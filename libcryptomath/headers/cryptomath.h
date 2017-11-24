/*! @file */ 
#pragma once

#include <iostream>

#ifndef DEBUG
    #define DBGOUT(a) 
#else
    #include <iostream>
    #define DBGOUT(a) std::cerr << a << std::endl;
#endif

#include "./math_factoring.h"
#include "./math_misc.h"
#include "./math_modulararith.h"
#include "./math_primality.h"