
# General-Purpose Cryptography Library
Written by Andrew Stelter

For Dr. Christer Karlsson

As a partial requirement of Cryptography (CSC 512), Fall 2017 at the South Dakota School of Mines and Technology

## Description
This library very closely
parallels the path that the course took through the subject. The module is 
structured such that an application which only needs to use part of it can
easily include only some of the code available. There are four main sections
to the library

    - Classic Crypto
    - Crypto Math
    - DES
    - Random

A number of tools were built using this project, also as a requirement of the class. They can be found in the
repository at https://github.com/Ipiano/crypto_tools

The book for the course was  "Introduction to Cryptography with Coding Theory (2nd Edition) 2nd Edition" by
Wade Trappe and Lawrence C. Washington. This book was the main reference used for writing the library.

DISCLAIMER: This library is not written to follow any specific cryptographic standards. It exists
to demonstrate an understanding of the concepts and theory which the subject is founded on.

### Personal Background
The requirements on this project were very general; all we were given was a list of things that it had to be capable
of doing. The language and user-interfaces were left completely open. I chose to do the project entirely in C++ for
a number of reasons
    
    - It seems to me, if you're writing a cryptographic library, you would want it to run quickly;
        C++ usually outperforms other languages
    - I knew using a multi-precision library would be useful, and I wanted to challenge myself
        to make the project with with or without it
    - I knew very little about writing makefiles (and still do, but now I know more), and since
        C++ is my language of choice, I wanted to learn some of the black magic that goes into them and
        come up with a modular way of writing makefiles so I can easily re-use my code libraries
        in the future
    
This project was a major learning experience for me; I'd never written any in-depth makefiles, or used git submodules before. Aside from the fact that I was learning all of the cryptography and coding theory for the first time, I was teaching myself a lot about some of the more tedious aspects of building a library like this.

I hope you find the project to be useful, either as a learning tool, as a starting point for some number-theory project you're doing, or even as an example of how to write ridiculous and convoluted makefiles. I've really enjoyed the experience of putting it all together (as stressful as it may have been at times) and think it could be a useful set of tools to have for myself and others.

### Classic Cryptography
The classic cryptography portion of the library contains encryption and decryption mechanisms for
the following ciphers, as well as a general-purpose frequency-analysis counter.

    - The German ADFGX Cipher
    - The Affine Cipher
    - The Vigenere Cipher

All three of these ciphers are structured the same way. Each has its own namespace, and within that
namespace is a class type 'transformer'. (i.e. adfgx::transformer). The transformer type is constructed
with the key and alphabet that should be used, and then can be used to encrypt or decrypt multiple texts.
This was done for efficiency's sake because there is often much error checking that needs to be done to ensure that a key 
is valid. More information on the specific ciphers can be found in the associated namespace and class descriptions.

### Crypto Math
The Crypto Math portion of the library makes up the majority of the project. The project
is intended to be usable with both standard C++ types and specialized Large-Number libraries. The
specific target library is GMP (The GNU Multi-precision Library), but adapting the project to another
should not be difficult.

The math library is split into a number of files for managability, but only once header included
is necessary to get all of them. (Specifically, #include "cryptomath.h").

    - Misc
    - Modular Arithmetic
    - Primality
    - Factoring
    - Continued Fractions
    - Specialization

Almost every function in the library is templated on type 'Integral', which
in most cases can be any Integer type which supports all the following functions and has template
specializations for all of the special-purpose functions. (Examples of these for GMP can be found
in the included specializations header)

    - operator +
    - operator -
    - operator /
    - operator *
    - operator %
    - abs(x)

The Miscellaneous header contains a number of small functions which are used to standardize
some calls which differ for standard number types and multi-precision libraries. This allows
the rest of the library to use all of the same template calls in either case.

The Modular Arithmetic header contains a numerically correct version of the modulus function,
which correctly handles negative values, as well as functions for finding

    - GCD
    - Extended GCD
    - Integer powers Mod n
    - Inverses Mod n
    - Legendre and Jacobi Symbols

The Primality header contains functions for testing and finding prime values.
Specifically it contains implementations of both the Miller-Rabin and Solovay-Strassen primality tests,
as well as supplemental functions for

    - Finding the first prime higher than a number
    - Generating random primes with specific numbers of bits
    - Generating lists of primes with the Sieve of Sundaram
    - Factoring all powers of 2 out of an even number

The Factoring header contains mainly a number of factorization algorithms and 
a factor() function which drives them to find all prime factors of a number. The specific factorization
algorithms available are

    - Fermat's Factorization
    - Shanks' Square Forms
    - Pollard's Rho algorithm
    - Pollard's P-1 algorithm
    
The additional functions in this header are a calculation of phi(x) and a function to test if
some x is a primitive root mod some n.

The Continued Fractions header and source follows a slightly different pattern from the rest of the sections.
It is the only part of the math library which is not header-only and templated. The continued fraction functions available
can be used to convert

    - Double to Continued Fraction
    - Simple Square root to Continued Fraction
    - Rational to Continued Fraction
    - Continued Fraction to Rational Approximations
    - Continued Fraction to Double

The Specializations portion contains all of the template specializations for the GMP mpz_class type. If you plan to
build your own specializations for a different Large Number library, I recommend you look through these functions. The
main differences between GMP and standard C++ types which necesitated these functions are

    - GMP does not have operators for bit-wise manipulation
    - GMP has special names for some functions (i.e mpz_sqrt)
    - GMP does not have a number size limit

### Random
The Random portion of the library contains cryptographically-secure pseudo-random number generation functions. The
only algorithm currently included here is the Blum Blum Shub algorithm.

### DES
The last portion of the library is the Data Encryption Standard (DES). While it has been shown to be insufficient for
good security, understanding the DES is still important. The reference book presented two versions of the DES: A simplified 
version that works on 12 bits, and the full 64-bit version. The book also outlined a couple of attack methods on the 
simplified version. Encryption and decryption functions for both versions are contained in this section, as well as
the 3-round and 4-round attackes presented for the simplified version.

## Unit Tests
The library contains a full suite of unit tests build using the Catch for C++ unit test framework. Every major
component of the library has a number of tests defined for it, and all the tests can be built both with and without
the GMP library.

Unit tests are contained in the unittests folder. The can be built from that folder with
```
make
```

The default build type is release, to trigger a debug build, add the make argument
```
BUILD_TYPE=debug
```
Debug builds of the unit tests will contain a verbose output; resulting in a much longer runtime.
The output executable will be placed in either the debug or release subdirectory of unittests, depending
on which version you build.

To build with GMP support, you must have previously installed GMP such that -lgmp and -lgmpxx can be
found by the linker. The GMP version can be built both debug and release with
```
make gmp
```

Because there are many objects built for the unit tests, you may desire to use the -j option to
parallelize the build. Unfortunately, at one point during development, the make scripts included
a number of bugs which prevented the -j option from working sometimes. It is unknown whether or not
this is resolved; if you attempt to use the -j flag and the build fails, try omitting the flag.

Regardless of whether you build with GMP or not, all object files are placed in the same directory.
(That directory is dependent upon whether the build is release or debug). If you build first GMP
and then non-GMP, the object files will not be rebuilt and linking will fail. To fix this, run
```
make clean
```
with the correct BUILD_TYPE flag to clear out all the old object files.

Once built, the unit tests can be run with
```
./release/unittests_modulecrypto
```
or
```
./debug/unittests_modulecrypto
```

As mentioned above, the debug version will take significantly longer to run. The non-GMP release version will
complete in a matter of seconds; however the GMP release version tests the Blum Blum Shub generator with some
very large numbers, and, on the laptop this library was developed on, runs for about a minute.

Example build commands
```
make -j gmp
make BUILD_TYPE=debug
make gmp BUILD_TYPE=debug
make clean
make clean BUILD_TYPE=debug
```

## Including the Library in a Project
The entire library was written so that it can be included in another project with minimal trouble. The process
involves first defining a number of variables in the main project Makefile, and then including the library's
Makefile 

    1. Add the library as a subfolder of the main project.
    2. Define the following variables in the main project Makefile
        - CRYPTO_ROOT - The root directory of the cryptography library
        - OBJECTS_DIR - The directory to output .o files to
        - LIBS_DIR - The directory to output .a files to
    3. Include the library's Makefile with

```
include $(CRYPTO_ROOT)/include.mk
```

If you would like to include only part of the library, define the variable CRYPTO_LIBS as a list of the features you would
like. The supported feature names parallel the folders in the library (random, cryptomath, classiccrypto, des). Some of these
features have sub-features which can be specified. To specify those, define the variable LIB[FEATURE]_FEATURES as a list of the
subfeatures. A list of subfeatures follows

    - Classic Crypto - affine, frequency, vigenere, adfgx
    - Random - bbs
    
If subfeatures are not specified, then all will be included.

Examples
```
CRYPTO_LIBS = des random # Will build/include only the DES and random libraries

CRYPTO_LIBS = des classiccrypto # Will build/include only the DES and Classic Crypto libraries
LIBCLASSICCRYPTO_FEATURES = affine frequency # Will build/include only the affine transform and frequency analysis from the Classic Crypto library
```

When the library makefile is included, a number of new variables will be defined or appended to which can be used to
properly define project dependencies.

    - INCLUDES will be appended to so that all headers included are directly in the include path
    - LIB_OBJECTS will be appended to to contain a list of all objects built. The list will contain the full file paths
    - LIB_HEADERS will contain a list of all header files associated with the included sections. The list will contain the full file paths

Additionally, each feature included will define OBJS_[FEATURE] and HDRS_[FEATURE], which are portions of the final LIB_OBJECTS and LIB_HEADERS
respectively. These can be used to fine-tune dependencies.

Lastly, there are a number of values that can be added to DEFINES before including the library makefile to modify what is included

    - DEFINES += -DCRYPTOMATH_GMP will add all GMP-specific code from the Crypto Math portion of the library
    - DEFINES += -DDEBUG will enable verbose debugging output from all parts of the library

For example Makefiles, see the unit tests makefile and all of the tools in the repo at https://github.com/Ipiano/crypto_tools

## Documentation
A full library API, which includes all the information in this readme, can be generated by running Doxygen from the root folder of the library.
The output will be placed in a folder called doc
