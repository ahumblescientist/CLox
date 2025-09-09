#include "chunk.h"
#include "memory.h"

void initChunk(Chunk *ch) {
	ch->code = NULL;
	ch->count = 0;
	ch->capacity = 0;
	initValueArray(&ch->constants);
	ch->lines = NULL;
}

void writeChunk(Chunk *ch, uint8_t wd, int line) {
	if(ch->count >= ch->capacity) {
		int oldC = ch->capacity;
		ch->capacity = GROW_CAPACITY(ch->capacity);
		ch->code = GROW_ARRAY(uint8_t, ch->code, oldC, ch->capacity);
		ch->lines = GROW_ARRAY(int, ch->lines, oldC, ch->capacity); 
	}
	ch->code[ch->count] = wd;
	ch->lines[ch->count] = line;
	ch->count++;
}

int addConstant(Chunk *ch, Value v) {
	writeValueArray(&ch->constants, v);
	return ch->constants.count - 1;
}

void freeChunk(Chunk *ch) {
	freeValueArray(&ch->constants);
	FREE_ARRAY(uint8_t, ch->code, ch->capacity);
	FREE_ARRAY(int, ch->lines, ch->capacity);
	initChunk(ch);
}
