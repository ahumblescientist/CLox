#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>

#define GROW_CAPACITY(c) ((c) < 8 ? 8 : 2*(c))
#define GROW_ARRAY(type, pointer, oldCapacity, newCapacity)\
	(type*)reallocate(pointer, sizeof(type) * (oldCapacity), sizeof(type) * (newCapacity))

#define FREE_ARRAY(type, pointer, size) \
	reallocate(pointer, sizeof(type) * size, 0)

void *reallocate(void *, size_t, size_t);

#endif
