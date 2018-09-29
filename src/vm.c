#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define _POSIX_C_SOURCE 200809L
#define STACK_SIZE 8192

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

typedef struct REGISTER_t {
	OBJECT value;
} REGISTER;

REGISTER* registers_new(int size){
	REGISTER* regs = (REGISTER*)malloc(sizeof(REGISTER) * size);
	return regs;
}

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

typedef uint8_t* (*instruction)(uint8_t *, STACK *, REGISTER *);

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
uint8_t *op_nop(uint8_t *ip, STACK *s, REGISTER* regs){
    return ip + 1;
}

// Push a character on the stack
uint8_t *op_pushc(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT o;
    o.type = CHAR;
    o.u8 = *(ip + 1);
    stack_push(s, o);
    return ip + 2;
}


// Pop x characters off the stack and print
uint8_t *op_emitc(uint8_t *ip, STACK *s, REGISTER* regs){
    uint32_t num;
	num = *((uint32_t*)(ip + 1));
	for(int i = 0; i < num; i++){
    	OBJECT o = stack_pop(s);
    	putchar(o.u8);
	}
    return ip + 5;
}

//print
uint8_t *op_printc(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT o = stack_peek(s);
    putchar(o.u8);
    return ip + 1;
}

// Push a number on the stack (double)
uint8_t *op_pushn(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT o;
    o.type = NUMBER;
    o.d64 = *((double*) (ip + 1));
    stack_push(s, o);
    return ip + 9;
}

// Pop x numbers off the stack and print
uint8_t *op_emitn(uint8_t *ip, STACK *s, REGISTER* regs){
	uint32_t num;
	num = *((uint32_t*)(ip + 1));
	for(int i = 0; i < num; i++){
    	OBJECT o = stack_pop(s);
    	printf("%f", o.d64);
	}
    return ip + 5;
}

// print
uint8_t *op_printn(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT o = stack_peek(s);
    printf("%f", o.d64);
    return ip + 1;
}

// Add the top 2 numbers on the stack
uint8_t *op_addn(uint8_t *ip, STACK *s, REGISTER* regs){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = NUMBER;
	o.d64 = b.d64 + a.d64;
	stack_push(s, o);
	return ip + 1;
}

// Subtract the top 2 numbers on the stack
uint8_t *op_subn(uint8_t *ip, STACK *s, REGISTER* regs){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = NUMBER;
	o.d64 = b.d64 - a.d64;
	stack_push(s, o);
	return ip + 1;
}

// Multiply the top 2 numbers on the stack
uint8_t *op_multn(uint8_t *ip, STACK *s, REGISTER* regs){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = NUMBER;
	o.d64 = b.d64 * a.d64;
	stack_push(s, o);
	return ip + 1;
}

// Divide the top two numbers on the stack
uint8_t *op_divn(uint8_t *ip, STACK *s, REGISTER* regs){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = NUMBER;
	o.d64 = b.d64 / a.d64;
	stack_push(s, o);
	return ip + 1;
}

// Jump to a new location
uint8_t *op_jump(uint8_t *ip, STACK *s, REGISTER* regs){
	int offset = *((uint32_t *)(ip + 1));
	return code + offset;
}

// Jump if number is equal to the number on the top of the stack
uint8_t *op_jumpe_n(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT a = stack_peek(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 == comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }

    return ip + 9;
}

// Jump if number is not equal to the number on the top of the stack
uint8_t *op_jumpne_n(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT a = stack_peek(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 != comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }

    return ip + 9;
}

// Jump if number is greater than the number on the top of the stack
uint8_t *op_jumpgt_n(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT a = stack_peek(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 > comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }

    return ip + 9;
}

// Jump if number is less than the number on the top of the stack
uint8_t *op_jumplt_n(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT a = stack_peek(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 < comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }
    return ip + 9;
}

// Jump if number is greater than the number on the top of the stack
uint8_t *op_jumpgte_n(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT a = stack_peek(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 >= comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }
    return ip + 9;
}

// Jump if number is less than the number on the top of the stack
uint8_t *op_jumplte_n(uint8_t *ip, STACK *s, REGISTER* regs){
    OBJECT a = stack_peek(s);
    double comp = *((double*) (ip + 1));
    if(a.d64 <= comp)
    {
        int offset = *((uint32_t *)(ip + 9));
        return code + offset;
    }

    return ip + 9;
}

// Get the current time in nanoseconds
uint8_t *op_nanotime(uint8_t *ip, STACK *s, REGISTER* regs){

    time_t t;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    double ns = (spec.tv_sec * 1000000000L) + spec.tv_nsec;

    OBJECT o;
    o.type = o.type = NUMBER;
    o.d64 = ns;
    stack_push(s, o);
    return ip + 1;

}

uint8_t *op_pop(uint8_t *ip, STACK *s, REGISTER* regs){
    stack_pop(s);
    return ip + 1;
}

uint8_t *op_grow(uint8_t *ip, STACK *s, REGISTER* regs){
	OBJECT o = stack_peek(s);
	stack_push(s, o);
	return ip + 1;
}

// Pop an object off the stack, then store in the specified register
uint8_t *op_popr(uint8_t *ip, STACK *s, REGISTER* regs){
	OBJECT o = stack_pop(s);
	uint32_t reg_id = *((uint32_t*)(ip + 1));
	regs[reg_id].value = o;
	
	return ip + 5;
}

// Push an object onto the stack from the specified register
uint8_t *op_pullr(uint8_t *ip, STACK *s, REGISTER* regs){
	uint32_t reg_id = *((uint32_t*)(ip + 1));
	OBJECT o = regs[reg_id].value;
	stack_push(s, o);
	return ip + 5;
}

int main(int argc, char** argv){
    uint8_t *ip;
    REGISTER *registers;

    
    if(argc != 2){
        usage();
    }
    
    for(int i = 0; i < 256; i++)
        ops[i] = op_nop;
        
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
    ops['S'] = op_popr;
    ops['R'] = op_pullr;
    
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
    
    code = load_file(argv[1]);
    data = stack_new(STACK_SIZE);
    ip = code;
    registers = registers_new(10);
    
    while(*ip != 'h'){
        ip = ops[*ip](ip, &data, registers);
        //sleep(1);
        //printf("->%i", ip);
    }
    
    return 0;
}

