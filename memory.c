#include "memory.h"


void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
	if(newSize == 0) {
		free(pointer);
		return NULL;
	}
	void *ret = realloc(pointer, newSize);
	if(ret == NULL) {
		exit(1);
	}
	return ret;
}

