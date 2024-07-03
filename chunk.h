#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include "value.h"

typedef enum {
	OP_CONSTANT,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_NOT,
	OP_GREATER,
	OP_EQUAL,
	OP_LESS,
	OP_PRINT,
	OP_POP,
	OP_DEFINE_GLOBAL,
	OP_GET_GLOBAL,
	OP_SET_GLOBAL,
	OP_RETURN,
} OpCode;

typedef struct {
	uint8_t *code;
	int count;
	int capacity;
	ValueArray varr;
	int *lines;
} Chunk;

void writeChunk(Chunk *, uint8_t, int);
void initChunk(Chunk *);
void freeChunk(Chunk *);
int addConstant(Chunk *, Value);

#endif
