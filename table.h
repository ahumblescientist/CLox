#ifndef TABLE_H
#define TABLE_H

#include "obj.h"

typedef struct {
	ObjString *key;
	Value value;
} Entry;

typedef struct {
	int count;
	int size;
	Entry *entries;
} Table;

void initTable(Table *);
void freeTable(Table *);
bool tableSet(Table *, ObjString *, Value value);
bool tableGet(Table *, ObjString *, Value *value);
void tableAddAll(Table *, Table *);
ObjString *tableFindString(Table *, char*, int, uint32_t);

#endif
