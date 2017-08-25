#include "algotest.h"
#include "gmpmodtest.h"

#include <iostream>

using namespace std;

void testalgorithmfunction(testtype a)
{
    cout << "This is a test algorithm function: " << a << endl;

    testgmpfunction();
}