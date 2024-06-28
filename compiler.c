#include "compiler.h"
#include "scanner.h"
#include <stdio.h>

void compile(char *source) {
	initScanner(source);
	int line=-1;
	while(1) {
		Token token = scanToken();
		printf("%2d %.*s\n", token.type, token.length, token.start);
		if(token.type == TOKEN_EOF) break;
	}

}


