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
	switch(v.type) {
		case VAL_BOOL:
			printf(AS_BOOL(v) ? "true" : "false");
			break;
		case VAL_NUMBER:
			printf("%g", AS_NUMBER(v));
			break;
		case VAL_NIL:
			printf("nil");
			break;
		case VAL_OBJ:
			printObj(v);
			break;
	}
}
