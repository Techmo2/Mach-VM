/*

    FILE: mobject.h

    DESCRIPTION:
        Defines the standard object to be used for data and computations
        within the Mach VM.

    AUTHOR: Bradly Tillmann

    MODIFIED: 17 February 2019

*/

#ifndef MACH_OBJECT_h
#define MACH_OBJECT_h

#include "mconfig.h"

typedef unsigned char MType;
typedef unsigned char MChar;
typedef char* MString;
typedef NUM_TYPE MNumber;

typedef struct {
    MType type;
    union {
        MChar c;
        MNumber n;
        MString s;
    };
} MObject;

#endif //MACH_OBJECT_h