#ifndef VM_H
#define VM_H

#include <stdint.h>
#include "table.h"
#include "chunk.h"
#include "value.h"

typedef struct {
	Chunk *chunk;
	uint8_t *ip;
	Value stack[256];
	Value *stackTop;
	Obj *objects;
	Table strings;
} VM;

extern VM vm;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;

void push(Value);
Value pop();
void initVM();
void freeVM();
InterpretResult interpret(char *);

#endif
