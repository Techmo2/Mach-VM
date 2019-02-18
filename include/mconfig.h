/*

    FILE: mconfig.h

    DESCRIPTION:
        Defines various constants and data types to be used within
        the Mach VM.

    AUTHOR: Bradly Tillmann

    MODIFIED: 17 February 2019

*/

#ifndef MACH_CONFIG_h
#define MACH_CONFIG_h

/* Type to be used for all numbers */
/* Mach programs will need this type specified during assembly */
#define NUM_TYPE double

/* How large the initial table should be */
#define M_TABLE_SIZE 1024

/* How large the initial stack should be */
#define M_STACK_SIZE 1024

// TODO
/* Grow the stack if it is filled? */
#define M_STACK_DYNAMIC true

// TODO
/* Grow the table if it is filled? */
#define M_TABLE_DYNAMIC true

#endif //MACH_CONFIG_h