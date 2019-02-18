#ifndef MACH_TABLE_h
#define MACH_TABLE_h

#include "mobject.h"

typedef struct {
    int size;
	MObject* table;
} MTable;

MTable table_new(int size);

void table_free(MTable* T);

#endif //MACH_TABLE_h