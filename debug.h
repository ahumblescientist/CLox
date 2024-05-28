#ifndef DEBUG_H
#define DEBUG_H

#include "chunk.h"
#include <stdio.h>

void disassembleChunk(Chunk *, char *);
int disassembleInstruction(Chunk *, int);

#endif
