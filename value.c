#include "value.h"
#include "memory.h"
#include <stdio.h>

void initValueArray(ValueArray *array) {
	array->count = 0;
	array->capacity = 0;
	array->values = NULL;
}

void writeValueArray(ValueArray *array, Value v) {
	if(array->count >= array->capacity) {
		int oldC = array->capacity;
		array->capacity = GROW_CAPACITY(array->capacity);
		array->values = GROW_ARRAY(Value, array->values, oldC, array->capacity);
	}
	array->values[array->count++] = v;
}

void freeValueArray(ValueArray *array) {
	FREE_ARRAY(Value, array->values, array->capacity);
	initValueArray(array);
}

void printValue(Value v) {
	printf("%g", v);
}
