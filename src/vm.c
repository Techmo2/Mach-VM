#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define STACK_SIZE 1024

enum TYPE {
    NUMBER = 'n',
    CHAR = 'c',
    POINTER = 'p'
};

typedef struct OBJECT_t {
    uint8_t type;
    union {
        uint8_t u8;
        int8_t i8;
        uint32_t u32;
        int32_t i32;
        double d64;
        void *ptr;
    };
} OBJECT;

typedef struct STACK_t {
    int top;
    int size;
    OBJECT *stack;
} STACK;

STACK stack_new(int size){
    STACK s;
    s.top = 0;
    s.size = size;
    s.stack = (OBJECT *)malloc(sizeof(OBJECT) * size);
    return s;
}

void stack_free(STACK *s){
    free(s->stack);
}

int stack_push(STACK *s, OBJECT o){
    s->stack[s->top++] = o;
    return s->top;
}

OBJECT stack_pop(STACK *s){
    return s->stack[--(s->top)];
}

OBJECT stack_peek(STACK *s){
    return s->stack[s->top - 1];
}

typedef uint8_t* (*instruction)(uint8_t *, STACK *);

STACK data;
instruction ops[256];
uint8_t *code;

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
uint8_t *op_nop(uint8_t *ip, STACK *s){
    return ip + 1;
}

// Push a character on the stack
uint8_t *op_pushc(uint8_t *ip, STACK *s){
    OBJECT o;
    o.type = CHAR;
    o.u8 = *(ip + 1);
    stack_push(s, o);
    return ip + 2;
}


// Pop character off the stack and print
uint8_t *op_emitc(uint8_t *ip, STACK *s){
    uint32_t num;
	num = *((uint32_t*)(ip + 1));
	for(int i = 0; i < num; i++){
    	OBJECT o = stack_pop(s);
    	putchar(o.u8);
	}
    return ip + 5;
}

// Push a number on the stack (double)
uint8_t *op_pushn(uint8_t *ip, STACK *s){
    OBJECT o;
    o.type = NUMBER;
    o.d64 = *((double*) (ip + 1));
    stack_push(s, o);
    return ip + 9;
}

// Pop a number off the stack and print
uint8_t *op_emitn(uint8_t *ip, STACK *s){
	uint32_t num;
	num = *((uint32_t*)(ip + 1));
	for(int i = 0; i < num; i++){
    	OBJECT o = stack_pop(s);
    	printf("%f", o.d64);
	}
    return ip + 5;
}

// Add the top 2 numbers on the stack
uint8_t *op_addn(uint8_t *ip, STACK *s){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = NUMBER;
	o.d64 = a.d64 + b.d64;
	stack_push(s, o);
	return ip + 1;
}

// Subtract the top 2 numbers on the stack
uint8_t *op_subn(uint8_t *ip, STACK *s){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = NUMBER;
	o.d64 = a.d64 - b.d64;
	stack_push(s, o);
	return ip + 1;
}

// Multiply the top 2 numbers on the stack
uint8_t *op_multn(uint8_t *ip, STACK *s){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = NUMBER;
	o.d64 = a.d64 * b.d64;
	stack_push(s, o);
	return ip + 1;
}

// Divide the top two numbers on the stack
uint8_t *op_divn(uint8_t *ip, STACK *s){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = NUMBER;
	o.d64 = a.d64 / b.d64;
	stack_push(s, o);
	return ip + 1;
}

// Jump to a new location
uint8_t *op_jump(uint8_t *ip, STACK *s){
	int offset = *((uint32_t *)(ip + 1));
	return code + offset;
}

// Jump if number is equal to the number on the top of the stack
uint8_t *op_jumpe_n(uint8_t *ip, STACK *s){
    OBJECT a = stack_pop(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 == comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }
}

// Jump if number is not equal to the number on the top of the stack
uint8_t *op_jumpne_n(uint8_t *ip, STACK *s){
    OBJECT a = stack_pop(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 != comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }
}

// Jump if number is greater than the number on the top of the stack
uint8_t *op_jumpgt_n(uint8_t *ip, STACK *s){
    OBJECT a = stack_pop(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 >= comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }
}

// Jump if number is less than the number on the top of the stack
uint8_t *op_jumplt_n(uint8_t *ip, STACK *s){
    OBJECT a = stack_pop(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 <= comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }
}

int main(int argc, char** argv){
    uint8_t *ip;

    
    if(argc != 2){
        usage();
    }
    
    for(int i = 0; i < 256; i++)
        ops[i] = op_nop;
        
    // IO
    ops['c'] = op_pushc;
    ops['e'] = op_emitc;
    ops['n'] = op_pushn;
    ops['v'] = op_emitn;
    
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
    
    code = load_file(argv[1]);
    data = stack_new(STACK_SIZE);
    ip = code;
    
    while(*ip != 'h'){
        ip = ops[*ip](ip, &data);
    }
    
    return 0;
}

