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


static ObjString *allocateString(char *chars, int length, uint32_t hash) {
	ObjString *ret = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	ret->length = length;
	ret->chars = chars;
	ret->hash = hash;
	tableSet(&vm.strings, ret, NIL_VAL);
	return ret;
}

static uint32_t hashString(char *start, int length) {
	uint32_t result = 0x811c9dc5; // some magic
	for(int i=0;i<length;i++) {
		result ^= start[i];
		result *= 0x01000193; // also some magic
	}
	return result;
}

ObjString *takeString(char *start, int length) {
	uint32_t hash = hashString(start, length);
	ObjString *interned = tableFindString(&vm.strings, start, length, hash);
	if(interned != NULL) {
		FREE_ARRAY(char, start, length+1);
		return interned;
	}
	return allocateString(start, length, hash);
}

ObjString *copyString(char *start, int length) {
	uint32_t hash = hashString(start, length);
	ObjString *interned = tableFindString(&vm.strings, start, length, hash);
	if(interned != NULL) return interned;
	char *heapChars = ALLOCATE(char, length+1);
	memcpy(heapChars, start, length);
	heapChars[length] = '\0';
	return allocateString(heapChars, length, hash);
}
