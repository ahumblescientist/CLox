#ifndef COMPILER_H
#define COMPILER_H

#include "chunk.h"
#include <stdbool.h>

bool compile(Chunk *chunk, char *source);

#endif
