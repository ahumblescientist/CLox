#include "debug.h"
#include <stdio.h>

void debugChunk(Chunk *ch, const char *name) {
	printf("=== %s ===\n", name);
	for(int offset = 0;offset < ch->count;) {
		offset = debugInstruction(ch, offset);
	}
}

static int simpleInstruction(const char *name, int offset) {
	printf("%s\n", name);
	return offset+1;
}

static int constantInstruction(const char *name, Chunk *ch, int offset) {
	uint8_t index = ch->code[offset+1];
	printf("%-16s %4d '", name, index);
	printValue(ch->constants.values[index]);
	printf("'\n");
	return offset + 2;
}

int debugInstruction(Chunk *ch, int offset) {
		printf("%04d ", offset);
	if(offset > 0 && ch->lines[offset] == ch->lines[offset-1]) {
		printf("     ");
	} else {
		printf("%04d ", ch->lines[offset]);
	}
	uint8_t instruction = ch->code[offset];
	switch(instruction) {
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", ch, offset);
		case OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);
		default:
			printf("Unkown instruction %d\n", instruction);
			return offset+1;
	}
}
