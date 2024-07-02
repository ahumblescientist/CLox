CFILES = main.c chunk.c memory.c debug.c value.c vm.c compiler.c scanner.c obj.c table.c
HFILES = Makefile chunk.h memory.h debug.h value.h vm.h compiler.h scanner.h obj.h table.h
FILES = $(CFILES) $(HFILES)
CC = clang
OUT = bin/main
FLAGS = -ggdb3 -O0 -o $(OUT)


main: $(FILES)
	$(CC) $(CFILES) $(FLAGS)
