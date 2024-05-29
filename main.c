#include <stdio.h>
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main() {
	Chunk chunk;
	initChunk(&chunk);
	initVM();
	writeChunk(&chunk, OP_CONSTANT, 1);
	int index = addConstant(&chunk, 69);
	writeChunk(&chunk, index, 1);
	writeChunk(&chunk, OP_CONSTANT, 1);
	index = addConstant(&chunk, 68);
	writeChunk(&chunk, index, 1);
	writeChunk(&chunk, OP_SUBTRACT, 1);
	writeChunk(&chunk, OP_RETURN, 1);
	interpret(&chunk);
	// disassembleChunk(&chunk, "main chunk");
	freeChunk(&chunk);
	freeVM();
	return 0;
}
