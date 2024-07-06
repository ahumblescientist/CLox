#include "vm.h"
#include "compiler.h"
#include <stdarg.h>
#include <string.h>
#include "memory.h"

#define TRACE_STACK
#undef TRACE_STACK

VM vm;

static void resetStack() {
	vm.stackTop = vm.stack;
}

void push(Value v) {
	*vm.stackTop = v;
	vm.stackTop++;
}

Value pop() {
	return *(--vm.stackTop);
}

static Value peek(int distance) {
	return *(vm.stackTop - 1 - distance);
}

static bool isTrue(Value v) {
	return (v.type != VAL_NIL && (v.type != VAL_BOOL || AS_BOOL(v)));
}

static void concatenate() {
	ObjString *b = AS_STRING(pop());
	ObjString *a = AS_STRING(pop());
	int length = a->length + b->length;
	char *chars = ALLOCATE(char, length+1);
	memcpy(chars, a->chars, a->length);
	memcpy(chars + a->length, b->chars, b->length);
	chars[length] = '\0';
	ObjString *result = takeString(chars, length);
	push(OBJ_VAL((Obj*)result));
}

static bool valuesEqual(Value a, Value b) {
	if(a.type != b.type) return false;
	switch(a.type) {
		case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
		case VAL_NIL: return true;
		case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
		case VAL_OBJ: return AS_OBJ(a) == AS_OBJ(b);
		default: return false;
	}
}

static void runtimeError(char *format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
	size_t instructionIndex = vm.ip - vm.chunk->code - 1;
	int line = vm.chunk->lines[instructionIndex];
	printf("[line %d] in script\n", line);
	resetStack();
}

void initVM() {
	vm.objects = NULL;
	initTable(&vm.strings);
	initTable(&vm.globals);
	resetStack();
}

void freeVM() {
	freeTable(&vm.strings);
	freeObjects();
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_SHORT() (((uint16_t)READ_BYTE() << 8) | (uint16_t)READ_BYTE())
#define READ_CONSTANT() (vm.chunk->varr.values[*vm.ip++])
#define READ_STRING() (AS_STRING(READ_CONSTANT()))
#define BINARY_OP(valueType, op) do {\
	if(!(IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))) {\
		runtimeError("Operands must be numbers");\
		return INTERPRET_RUNTIME_ERROR;\
	}\
	double b = AS_NUMBER(pop()); \
	double a = AS_NUMBER(pop()); \
	push(valueType(a op b));\
} while(0)

	while(1) {
#ifdef TRACE_STACK
		for(int i=0;i<vm.stackTop-vm.stack;i++) {
			printValue(vm.stack[i]);
			printf("\n");
		}
#endif
		uint8_t instruction;
		switch(instruction = READ_BYTE()) {
			case OP_CONSTANT: {
				Value constant = READ_CONSTANT();
				push(constant);
				break;
				}
			case OP_RETURN:
				return INTERPRET_OK;
			case OP_NEGATE: {
				Value T = peek(0);
				if(!IS_NUMBER(T)) {
					runtimeError("Operand must be a number.");
					return INTERPRET_RUNTIME_ERROR;
				}
				pop();
				push(NUMBER_VAL(-AS_NUMBER(T)));
				break;
			}
			case OP_ADD: {
				if(IS_STRING(peek(0)) && IS_STRING(peek(1))) {
					concatenate();
				} else if(IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
					BINARY_OP(NUMBER_VAL, +);
				} else {
					runtimeError("Operands must be numebrs or strings.");
					return INTERPRET_RUNTIME_ERROR;
				}
				break;
			}
			case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
			case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
			case OP_DIVIDE: 	BINARY_OP(NUMBER_VAL, /); break;
			case OP_TRUE: push(BOOL_VAL(true)); break;
			case OP_FALSE: push(BOOL_VAL(false)); break;
			case OP_NIL: push(NIL_VAL); break;
			case OP_NOT: {
				push(BOOL_VAL(!isTrue(pop())));
				break;
			}
			case OP_EQUAL: {
				Value a = pop();
				Value b = pop();
				push(BOOL_VAL(valuesEqual(a, b)));
				break;
			}
			case OP_GREATER: BINARY_OP(BOOL_VAL, >); break;
			case OP_LESS: BINARY_OP(BOOL_VAL, <); break;
			case OP_PRINT: {
				printValue(pop());
				printf("\n");
				break;
			}
			case OP_DEFINE_GLOBAL: {
				ObjString *name = READ_STRING();
				tableSet(&vm.globals, name, pop());
				break;
			}
			case OP_GET_GLOBAL: {
				ObjString *name = READ_STRING();
				Value temp;
				if(!tableGet(&vm.globals, name, &temp)) {
					runtimeError("Refrence to undefined variable '%s'", name->chars);
					return INTERPRET_RUNTIME_ERROR;
				} else {
					push(temp);
				}
				break;
			}
			case OP_SET_GLOBAL: {
				ObjString *name = READ_STRING();
				if(tableSet(&vm.globals, name, peek(0))) {
					runtimeError("Undefined variable '%s'", name->chars);
					return INTERPRET_RUNTIME_ERROR;
				} 
				break;
			}
			case OP_POP: pop(); break;
			case OP_GET_LOCAL: {
				int index = READ_BYTE();
				push(vm.stack[index]);
				break;
			}
			case OP_SET_LOCAL: {
				int index = READ_BYTE();
				vm.stack[index] = peek(0);
				break;
			}
			case OP_JUMP_IF_FALSE: {
				Value condition = peek(0);
				uint16_t offset = READ_SHORT();
				if(!isTrue(condition)) {
					vm.ip += offset;
				}
				break;
			}
			case OP_JUMP: {
				uint16_t offset = READ_SHORT();
				vm.ip += offset;
				break;
			}
			case OP_LOOP: {
				uint16_t offset = READ_SHORT();
				vm.ip -= offset;
				break;
			}

		}
	}
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
#undef READ_STRING
#undef READ_SHORT
}

InterpretResult interpret(char *source) {
	Chunk chunk;
	initChunk(&chunk);
	if(!compile(&chunk, source)) {
		return INTERPRET_COMPILE_ERROR;
	}
	vm.chunk = &chunk;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();
	freeChunk(&chunk);
	return INTERPRET_OK; 
}
