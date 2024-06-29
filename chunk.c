#include "chunk.h"
#include "memory.h"


void initChunk(Chunk *chunk) {
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;
	chunk->lines = NULL;
	initValueArray(&chunk->varr);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
	if(chunk->capacity <= chunk->count) {
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}
	chunk->code[chunk->count] = byte;
	chunk->lines[chunk->count++] = line;
}

int addConstant(Chunk *chunk, Value val) {
	writeValueArray(&chunk->varr, val);
	return chunk->varr.count-1;
}

void freeChunk(Chunk *chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	freeValueArray(&chunk->varr);
	initChunk(chunk);
}
