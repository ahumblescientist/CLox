CFILES = main.c chunk.c memory.c debug.c value.c
HFILES = Makefile chunk.h memory.h common.h debug.h value.h
FILES = $(CFILES) $(HFILES)
CC = clang
OUT = bin/main
FLAGS = -ggdb3 -O3 -lm -o $(OUT)


main: $(FILES)
	$(CC) $(CFILES) $(FLAGS)
