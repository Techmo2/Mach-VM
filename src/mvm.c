#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "mobject.h"
#include "mstack.h"
#include "mtable.h"

#define _POSIX_C_SOURCE 200809L

enum TYPE {
    NUMBER = 'n',
    CHAR = 'c',
    POINTER = 'p',
    STRING = 's'
};

void p(int n){
    printf("%i\n", n);
}

// Mach VM State
typedef struct {
    uint8_t* ip; // Instruction Pointer
    uint8_t* bc; // Bytecode
    MStack s; // Stack
    MTable t; // Table
    int halt; // Halt flag
} MState;

typedef uint8_t* (*instruction)(MState*);

void usage(){
    printf("usage: mach <file>\n");
    exit(1);
}

uint8_t* load_file(char *filename){
    FILE *f;
    int size;
    uint8_t *code = NULL;
    struct stat st;
    
    if((f = fopen(filename, "r"))){
        fstat(fileno(f), &st);
        code = (uint8_t*)malloc(st.st_size);
        fread((void *)code, 1, st.st_size, f);
    }
    else{
        printf("ERR: failed to open file %s\n", filename);
        usage();
    }
    
    return code;
}

// No Operation
uint8_t *op_nop(MState* state){
    return state->ip + 1;
}

// Push a character on the stack
uint8_t *op_pushc(MState* state){
    MObject o;
    o.type = CHAR;
    o.c = *(state->ip + 1);
    stack_push(&state->s, o);
    return state->ip + 2;
}


// Pop x characters off the MStack and print
uint8_t *op_emitc(MState* state){
    int num;
	num = *((int*)(state->ip + 1));
	for(int i = 0; i < num; i++){
    	MObject o = stack_pop(&state->s);
    	putchar(o.c);
	}
    return state->ip + 5;
}

//print
uint8_t *op_printc(MState* state){
    MObject o = stack_peek(&state->s);
    putchar(o.c);
    return state->ip + 1;
}

// Push a number on the stack
uint8_t *op_pushn(MState* state){
    MObject o;
    o.type = NUMBER;
    o.n = *((NUM_TYPE*) (state->ip + 1));
    stack_push(&state->s, o);
    return state->ip + 1 + sizeof(NUM_TYPE);
}

// Pop x numbers off the MStack and print
uint8_t *op_emitn(MState* state){
	int num;
	num = *((int*)(state->ip + 1));
	for(int i = 0; i < num; i++){
    	MObject o = stack_pop(&state->s);
    	printf("%f", o.n);
	}
    return state->ip + 5;
}

// print
uint8_t *op_printn(MState* state){
    MObject o = stack_peek(&state->s);
    printf("%f", o.n);
    return state->ip + 1;
}

// Add the top 2 numbers on the stack
uint8_t *op_addn(MState* state){
	MObject a = stack_pop(&state->s);
	MObject b = stack_pop(&state->s);
	MObject o;
	o.type = NUMBER;
	o.n = b.n + a.n;
	stack_push(&state->s, o);
	return state->ip + 1;
}

// Subtract the top 2 numbers on the stack
uint8_t *op_subn(MState* state){
	MObject a = stack_pop(&state->s);
	MObject b = stack_pop(&state->s);
	MObject o;
	o.type = NUMBER;
	o.n = b.n - a.n;
	stack_push(&state->s, o);
	return state->ip + 1;
}

// Multiply the top 2 numbers on the stack
uint8_t *op_multn(MState* state){
	MObject a = stack_pop(&state->s);
	MObject b = stack_pop(&state->s);
	MObject o;
	o.type = NUMBER;
	o.n = b.n * a.n;
	stack_push(&state->s, o);
	return state->ip + 1;
}

// Divide the top two numbers on the stack
uint8_t *op_divn(MState* state){
	MObject a = stack_pop(&state->s);
	MObject b = stack_pop(&state->s);
	MObject o;
	o.type = NUMBER;
	o.n = b.n / a.n;
	stack_push(&state->s, o);
	return state->ip + 1;
}

// Jump to a new location
uint8_t *op_jump(MState* state){
	int offset = *((int *)(state->ip + 1));
	return state->bc + offset;
}

// Jump if number is equal to the number on the top of the stack
uint8_t *op_jumpe_n(MState* state){
    MObject a = stack_peek(&state->s);
    NUM_TYPE comp = *((NUM_TYPE*) (state->ip + 1));
    if(a.n == comp)
    {
        int offset = *((int *)(state->ip + 1 + sizeof(NUM_TYPE)));
        return state->bc + offset;
    }

    return state->ip + 1 + sizeof(NUM_TYPE);
}

// Jump if number is not equal to the number on the top of the stack
uint8_t *op_jumpne_n(MState* state){
    MObject a = stack_peek(&state->s);
    NUM_TYPE comp = *((NUM_TYPE*) (state->ip + 1));
    if(a.n != comp)
    {
        int offset = *((int *)(state->ip + 1 + sizeof(NUM_TYPE)));
        return state->bc + offset;
    }

    return state->ip + 1 + sizeof(NUM_TYPE);
}

// Jump if number is greater than the number on the top of the stack
uint8_t *op_jumpgt_n(MState* state){
    MObject a = stack_peek(&state->s);
    NUM_TYPE comp = *((NUM_TYPE*) (state->ip + 1));
    if(a.n > comp)
    {
        int offset = *((int *)(state->ip + 1 + sizeof(NUM_TYPE)));
        return state->bc + offset;
    }

    return state->ip + 1 + sizeof(NUM_TYPE);
}

// Jump if number is less than the number on the top of the stack
uint8_t *op_jumplt_n(MState* state){
    MObject a = stack_peek(&state->s);
    NUM_TYPE comp = *((NUM_TYPE*) (state->ip + 1));
    if(a.n < comp)
    {
        int offset = *((int *)(state->ip + 1 + sizeof(NUM_TYPE)));
        return state->bc + offset;
    }
    return state->ip + 1 + sizeof(NUM_TYPE);
}

// Jump if number is greater than the number on the top of the stack
uint8_t *op_jumpgte_n(MState* state){
    MObject a = stack_peek(&state->s);
    NUM_TYPE comp = *((NUM_TYPE*) (state->ip + 1));
    if(a.n >= comp)
    {
        int offset = *((int *)(state->ip + 1 + sizeof(NUM_TYPE)));
        return state->bc + offset;
    }
    return state->ip + 1 + sizeof(NUM_TYPE);
}

// Jump if number is less than the number on the top of the stack
uint8_t *op_jumplte_n(MState* state){
    MObject a = stack_peek(&state->s);
    NUM_TYPE comp = *((NUM_TYPE*) (state->ip + 1));
    if(a.n <= comp)
    {
        int offset = *((int *)(state->ip + 1 + sizeof(NUM_TYPE)));
        return state->bc + offset;
    }

    return state->ip + 1 + sizeof(NUM_TYPE);
}

// Get the current time in nanoseconds
uint8_t *op_nanotime(MState* state){

    time_t t;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    NUM_TYPE ns = (spec.tv_sec * 1000000000L) + spec.tv_nsec;

    MObject o;
    o.type = o.type = NUMBER;
    o.n = ns;
    stack_push(&state->s, o);
    return state->ip + 1;

}

// Pop an object off the stack
uint8_t *op_pop(MState* state){
    stack_pop(&state->s);
    return state->ip + 1;
}

// Clone the object on the top of the stack
uint8_t *op_grow(MState* state){
	MObject o = stack_peek(&state->s);
	stack_push(&state->s, o);
	return state->ip + 1;
}

// Pop an object off the stack, then store in the specified register
uint8_t *op_popt(MState* state){
	MObject o = stack_pop(&state->s);
	int idx = *((int*)(state->ip + 1));
	state->t.table[idx] = o;
	
	return state->ip + 5;
}

// Push an object onto the MStack from the specified register
uint8_t *op_pullt(MState* state){
	int idx = *((int*)(state->ip + 1));
	MObject o = state->t.table[idx];
	stack_push(&state->s, o);
	return state->ip + 5;
}

// Push a string object onto the stack
uint8_t *op_pushs(MState* state){
    int slen = *((int*)(state->ip + 1));
    MObject o;
    o.type = STRING;

    char s[slen + 1];
    s[slen] = '\0';

    int s_idx = 0;

    while(s_idx < slen){
        char c = *((char*)(state->ip + 5 + s_idx));
        s[s_idx++] = c;
    }

    return state->ip + 5 + slen;
}

// Pop a string off the stack and prints
uint8_t *op_emits(MState* state){
    MObject o = stack_pop(&state->s);
    printf("%s", o.s);

    return state->ip + 1;
}

// Prints a string at the top of the stack
uint8_t *op_prints(MState* state){
    MObject o = stack_peek(&state->s);
    printf("%s", o.s);

    return state->ip + 1;
}

// Set the halt flag to true
uint8_t *op_halt(MState* state){
    state->halt = 1;
}

int main(int argc, char** argv){
    
    if(argc != 2){
        usage();
    }

    // Instruction index
    instruction ops[256];

    // Create VM state
    MState state;
    state.halt = 0;
    state.s = stack_new(M_STACK_SIZE);
    state.t = table_new(M_TABLE_SIZE);
    
    // Set all instructions to nop
    for(int i = 0; i < 256; i++)
        ops[i] = op_nop;
        
    /* Set indices in instruction table */
    // IO
    ops['c'] = op_pushc;
    ops['e'] = op_emitc;
    ops['E'] = op_printc;
    ops['n'] = op_pushn;
    ops['v'] = op_emitn;
    ops['V'] = op_printn;
    ops['r'] = op_nanotime;
    ops['p'] = op_pop;
    ops['b'] = op_grow;
    ops['S'] = op_popt;
    ops['R'] = op_pullt;
    
    // Math
    ops['m'] = op_multn;
    ops['a'] = op_addn;
    ops['s'] = op_subn;
    ops['d'] = op_divn;
    
    // Control
    ops['j'] = op_jump;
    ops['l'] = op_jumplt_n;
    ops['g'] = op_jumpgt_n;
    ops['t'] = op_jumpe_n;
    ops['f'] = op_jumpne_n;
    ops['L'] = op_jumplte_n;
    ops['G'] = op_jumpgte_n;
    ops['h'] = op_halt;
    
    // Load bytecode
    state.bc = load_file(argv[1]);

    // Set instruction pointer to beginning of bytecode
    state.ip = state.bc;
    
    while(!state.halt){
        state.ip = ops[*state.ip](&state);
    }

    // Free allocations
    stack_free(&state.s);
    table_free(&state.t);
    free(state.bc);
    
    return 0;
}

