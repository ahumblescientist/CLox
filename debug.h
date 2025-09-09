#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include "chunk.h"

void debugChunk(Chunk *chunk, const char *name);
int debugInstruction(Chunk *chunk, int offset);

#endif
