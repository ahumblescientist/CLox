#include "memory.h"
#include <stdlib.h>

void *reallocate(void *ptr, size_t oldsize, size_t newsize) {
	if(newsize == 0) {
		free(ptr);
		return NULL;
	}
	void *newptr = realloc(ptr, newsize);
	if(newptr == NULL) exit(1);
	return newptr;
}
