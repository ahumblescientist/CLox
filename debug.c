#include "debug.h"

static int simpleInstruction(char *name, int offset) {
	printf("%s\n", name);
	return offset+1;
}

static int constantInstruction(char *name, Chunk *chunk, int offset) {
	int index = chunk->code[offset+1];
	printf("%s %i ", name, index);
	printValue(chunk->varr.values[index]);
	printf("\n");
	return offset+2;
}

int disassembleInstruction(Chunk *chunk, int offset) {
	printf("%04d ", offset);
	uint8_t instruction = chunk->code[offset];
	printf("%i ", chunk->lines[offset]);
	switch(instruction) {
		case OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);
		default:
			printf("unknown opcode %d \n", instruction);
			return offset+1;
	}
}


void disassembleChunk(Chunk *chunk, char *name) {
	printf("=== %s ===\n", name);
	for(int offset=0;offset<chunk->count;) {
		offset = disassembleInstruction(chunk, offset);
	}
}

