#include <stdio.h>
#include "chunk.h"
#include "debug.h"

int main() {
	Chunk chunk;
	initChunk(&chunk);
	writeChunk(&chunk, OP_CONSTANT, 1);
	int index = addConstant(&chunk, 69);
	writeChunk(&chunk, index, 1);
	disassembleChunk(&chunk, "main chunk");
	freeChunk(&chunk);
	return 0;
}
