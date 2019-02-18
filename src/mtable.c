#include "mtable.h"
#include "malloc.h"

MTable table_new(int size){
    MTable t;
    t.size = size;
    t.table = (MObject *)malloc(sizeof(MObject) * size);
    return t;
}

void table_free(MTable* T){
    free(T->table);
}