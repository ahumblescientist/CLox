#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>

#include "vm.h"
#include "obj.h"

#define ALLOCATE(type, size) (type*)reallocate(NULL, 0, size*sizeof(type))
#define ALLOCATE_OBJ(type, objType) (type *)allocateObject(sizeof(type), objType)

#define GROW_CAPACITY(c) ((c) < 8 ? 8 : 2*(c))
#define GROW_ARRAY(type, pointer, oldCapacity, newCapacity)\
	(type*)reallocate(pointer, sizeof(type) * (oldCapacity), sizeof(type) * (newCapacity))

#define FREE_ARRAY(type, pointer, size) \
	reallocate(pointer, sizeof(type) * size, 0)

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

void *reallocate(void *, size_t, size_t);

void freeObjects();

#endif
