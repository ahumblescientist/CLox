#ifndef CLOX_MEMORY_H
#define CLOX_MEMORY_H

#include "common.h"

#define GROW_CAPACITY(cap) \
	((cap < 8) ? 8 : (2 * cap))

#define GROW_ARRAY(type, oldPtr, oldCnt, newCnt) \
	(type*)reallocate(oldPtr, sizeof(type)*(oldCnt), sizeof(type)*(newCnt));

#define FREE_ARRAY(type, oldPtr, oldCnt) \
	reallocate(oldPtr, sizeof(type)*(oldCnt), 0)

void *reallocate(void *, size_t, size_t);

#endif
