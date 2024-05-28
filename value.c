#include "value.h"
#include "memory.h"

void initValueArray(ValueArray *varr) {
	varr->count = 0;
	varr->capacity = 0;
	varr->values = NULL;
}

void freeValueArray(ValueArray *varr) {
	FREE_ARRAY(Value, varr->values, varr->capacity);
	initValueArray(varr);
}

void writeValueArray(ValueArray *varr, Value value) {
	if(varr->capacity <= varr->count) {
		int oldCapacity = varr->capacity;
		varr->capacity = GROW_CAPACITY(oldCapacity);
		varr->values = GROW_ARRAY(Value, varr->values, oldCapacity, varr->capacity);
	}
	varr->values[varr->count++] = value;
}

void printValue(Value v) {
	printf("%g", v);
}
