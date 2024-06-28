#include "vm.h"
#include "compiler.h"

VM vm;

#define TRACE_STACK

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


void initVM() {
	resetStack();
}

void freeVM() {
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->varr.values[*vm.ip++])
#define BINARY_OP(op) do {\
	double b = pop(); \
	double a = pop(); \
	push(a op b);\
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
				Value T = pop();
				push(-T);
				break;
			}
			case OP_ADD: BINARY_OP(+); break;
			case OP_SUBTRACT: BINARY_OP(-); break;
			case OP_MULTIPLY: BINARY_OP(*); break;
			case OP_DIVIDE: BINARY_OP(/); break;
		}
	}
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(char *source) {
	compile(source);
	return INTERPRET_OK; 
}
