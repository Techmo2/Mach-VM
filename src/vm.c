#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define STACK_SIZE 1024

typedef struct OBJECT_t {
    uint8_t type;
    union {
        uint8_t u8;
        int8_t i8;
        uint32_t u32;
        int32_t i32;
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

uint8_t *op_nop(uint8_t *ip, STACK *s){
    return ip + 1;
}

uint8_t *op_pushc(uint8_t *ip, STACK *s){
    OBJECT o;
    o.type = 'c';
    o.u8 = *(ip + 1);
    stack_push(s, o);
    return ip + 2;
}

uint8_t *op_emitc(uint8_t *ip, STACK *s){
    OBJECT o = stack_pop(s);
    putchar(o.u8);
    return ip + 1;
}

uint8_t *op_pushi(uint8_t *ip, STACK *s){
    OBJECT o;
    o.type = 'i';
    o.u32 = *((uint32_t*) (ip + 1));
    stack_push(s, o);
    return ip + 5;
}

uint8_t *op_emiti(uint8_t *ip, STACK *s){
    OBJECT o = stack_pop(s);
    printf("%u", o.u32);
    return ip + 1;
}

uint8_t *op_addi(uint8_t *ip, STACK *s){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = 'i';
	o.i32 = a.i32 + b.i32;
	stack_push(s, o);
	return ip + 1;
}

uint8_t *op_subi(uint8_t *ip, STACK *s){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = 'i';
	o.i32 = a.i32 - b.i32;
	stack_push(s, o);
	return ip + 1;
}

uint8_t *op_multi(uint8_t *ip, STACK *s){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = 'i';
	o.i32 = a.i32 * b.i32;
	stack_push(s, o);
	return ip + 1;
}

uint8_t *op_divi(uint8_t *ip, STACK *s){
	OBJECT a = stack_pop(s);
	OBJECT b = stack_pop(s);
	OBJECT o;
	o.type = 'i';
	o.i32 = a.i32 / b.i32;
	stack_push(s, o);
	return ip + 1;
}

uint8_t *op_jump(uint8_t *ip, STACK *s){
	int offset = *((uint32_t *)(ip + 1));
	return code + offset;
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
    ops['i'] = op_pushi;
    ops['n'] = op_emiti;
    
    // Math
    ops['m'] = op_multi;
    ops['a'] = op_addi;
    ops['s'] = op_subi;
    ops['d'] = op_divi;
    
    // Control
    ops['j'] = op_jump;
    
    code = load_file(argv[1]);
    data = stack_new(STACK_SIZE);
    ip = code;
    
    while(*ip != 'h'){
        ip = ops[*ip](ip, &data);
    }
    
    return 0;
}

