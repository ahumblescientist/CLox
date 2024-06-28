#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"
#include "debug.h"
#include "vm.h"

char *readFile(char *filename) {
	FILE *file = fopen(filename, "rb");
	if(file == NULL) {
		printf("Could not open file %s\n", filename);
		exit(74);
	}
	fseek(file, 0L, SEEK_END);
	size_t filesize = ftell(file);
	rewind(file);
	char *buffer = (char *)malloc(filesize + 1);
	if(buffer == NULL) {
		printf("Could not allocate enough memory\n");
	}
	size_t bytesRead = fread(buffer, sizeof(char), filesize, file);
	if(bytesRead < filesize) {
		printf("Could not read file %s\n", filename);
	}

	buffer[filesize] = '\0';
	fclose(file);
	return buffer;
}

static void runFile(char *filename) {
	char *source = readFile(filename);
	InterpretResult result = interpret(source);
	free(source);
	if(result == INTERPRET_COMPILE_ERROR) exit(65);
	if(result == INTERPRET_RUNTIME_ERROR) exit(70);
}


static void repl() {
	char line[1024];
	while(1) {
		printf("> ");
		if(!fgets(line, sizeof(line), stdin)) {
			printf("\n");
			break;
		}
		interpret(line);
	}
}

int main(int argc, char **argv) {
	if(argc==1) {
		repl();
	} else if(argc == 2) {
		runFile(argv[1]);
	} else {
		printf("Usage: clox [path]\n");
		exit(64);
	}
	freeVM();
	return 0;
}
