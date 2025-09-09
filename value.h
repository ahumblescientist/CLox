#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"

typedef double Value;

typedef struct {
	int capacity;
	int count;
	Value *values;
} ValueArray;

void initValueArray(ValueArray *);
void writeValueArray(ValueArray *, Value);
void freeValueArray(ValueArray *);
void printValue(Value);

#endif
