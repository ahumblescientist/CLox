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

static void freeObject(Obj *obj) {
	switch(obj->type) {
		case OBJ_STRING: {
			ObjString *string = (ObjString *)obj;
			FREE_ARRAY(char, string->chars, string->length);
			FREE(ObjString, obj);
			break;
		}
	}
}

void freeObjects() {
	Obj *object = vm.objects;
	while(object != NULL) {
		Obj *toFree = object;
		object = object->next;
		freeObject(toFree);
	}
}
