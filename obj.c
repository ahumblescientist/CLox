#include "obj.h"
#include "memory.h"
#include <string.h>

void printObj(Value value) {
	switch(OBJ_TYPE(value)) {
		case OBJ_STRING: printf("%s", AS_CSTRING(value)); break;
	}
}

Obj *allocateObject(size_t size, ObjType type) {
	Obj *obj = (Obj *)reallocate(NULL, 0, size);
	obj->type = type;
	obj->next = vm.objects;
	vm.objects = obj;
	return obj;
}


static ObjString *allocateString(char *chars, int length) {
	ObjString *ret = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	ret->length = length;
	ret->chars = chars;
	return ret;
}

ObjString *takeString(char *start, int length) {
	return allocateString(start, length);
}

ObjString *copyString(char *start, int length) {
	char *heapChars = ALLOCATE(char, length+1);
	memcpy(heapChars, start, length);
	heapChars[length] = '\0';
	return allocateString(heapChars, length);
}
