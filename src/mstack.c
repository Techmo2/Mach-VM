#include "mstack.h"
#include "malloc.h"


MStack stack_new(int size){
    MStack s;
    s.top = 0;
    s.size = size;
    s.stack = (MObject *)malloc(sizeof(MObject) * size);
    return s;
}

void stack_free(MStack *s){
    free(s->stack);
}

int stack_push(MStack *s, MObject o){
    s->stack[s->top++] = o;
    return s->top;
}

MObject stack_pop(MStack *s){
    return s->stack[--(s->top)];
}

MObject stack_peek(MStack *s){
    return s->stack[s->top - 1];
}