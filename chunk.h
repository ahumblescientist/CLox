#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
	OP_CONSTANT,
	OP_RETURN,
} OpCode;

typedef struct {
	uint8_t *code;
	int count;
	int capacity;
	ValueArray constants;
	int *lines;
} Chunk;

void initChunk(Chunk *);
void writeChunk(Chunk *, uint8_t data, int line);
int addConstant(Chunk *, Value v);
void freeChunk(Chunk *);

#endif
