#ifndef VALUE_H
#define VALUE_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {
	VAL_NUMBER,
	VAL_BOOL,
	VAL_NIL,
} ValueType;

typedef struct {
	ValueType type;
	union {
		bool Boolean;
		double Number;
	} as;
} Value;

#define BOOL_VAL(b) ((Value){VAL_BOOL, {.Boolean=b}})
#define NUMBER_VAL(b) ((Value){VAL_NUMBER, {.Number=b}})
#define NIL_VAL ((Value){VAL_NIL, {.Number = 0}})

#define AS_BOOL(v) (v.as.Boolean)
#define AS_NUMBER(v) (v.as.Number)

#define IS_NIL(v) (v.type == VAL_NIL)
#define IS_BOOL(v) (v.type == VAL_BOOL)
#define IS_NUMBER(v) (v.type == VAL_NUMBER)

typedef struct {
	int capacity;
	int count;
	Value *values;
} ValueArray;


void initValueArray(ValueArray *);
void freeValueArray(ValueArray *);
void writeValueArray(ValueArray *, Value);
void printValue(Value v);

#endif
