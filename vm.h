#ifndef VM_H
#define VM_H

#include <stdint.h>
#include "chunk.h"
#include "value.h"


typedef struct {
	Chunk *chunk;
	uint8_t *ip;
	Value stack[256];
	Value *stackTop;
} VM;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpreterResult;

void push(Value);
Value pop();
void initVM();
void freeVM();
InterpreterResult interpret(Chunk *);

#endif
