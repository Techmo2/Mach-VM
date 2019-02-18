/*

    FILE: mstack.h

    DESCRIPTION:
        Defines the structure and function of the global stack used within
        the Mach VM.

    AUTHOR: Bradly Tillmann

    MODIFIED: 17 February 2019

*/

#include "mobject.h"

typedef struct {
    int top;
    int size;
    MObject *stack;
} MStack;

MStack stack_new(int size);

void stack_free(MStack *s);

int stack_push(MStack *s, MObject o);

MObject stack_pop(MStack *s);

MObject stack_peek(MStack *s);