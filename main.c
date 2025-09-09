#include <stdio.h>
#include "chunk.h"
#include "debug.h"

int main() {
	Chunk ch;
	initChunk(&ch);
	writeChunk(&ch, OP_RETURN, 1);
	writeChunk(&ch, OP_CONSTANT, 1);
	int index = addConstant(&ch, 420);
	writeChunk(&ch, index, 2);
	debugChunk(&ch, "FIRST DEBUG");
	freeChunk(&ch);
	return 0;
}
