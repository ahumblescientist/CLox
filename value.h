#ifndef VALUE_H
#define VALUE_H

#include <stdio.h>

typedef double Value;

typedef struct {
	int capacity;
	int count;
	Value *values;
} ValueArray;

void initValueArray(ValueArray *);
void freeValueArray(ValueArray *);
void writeValueArray(ValueArray *, Value);
void printValue(Value v);

#endif
