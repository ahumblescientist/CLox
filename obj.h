#ifndef OBJ_H
#define OBJ_H

#include "value.h"


#define OBJ_TYPE(v) (AS_OBJ(v)->type)


#define IS_STRING(v) isObjType(v, OBJ_STRING)
#define AS_STRING(v) ((ObjString*)AS_OBJ(v))
#define AS_CSTRING(v) (((ObjString*)AS_OBJ(v))->chars)

ObjString *copyString(char *chars, int length);
ObjString *takeString(char*, int);
void printObj(Value value);

typedef enum {
	OBJ_STRING,
} ObjType;

struct Obj {
	ObjType type;
	struct Obj *next;
};

struct ObjString {
	Obj *obj;
	int length;
	char *chars;
};


Obj *allocateObject(size_t, ObjType);

static inline bool isObjType(Value value, ObjType type) {
	return IS_OBJ(value) && (OBJ_TYPE(value)) == type;
}

#endif
