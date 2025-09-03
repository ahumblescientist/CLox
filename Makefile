CFILES = main.c
HFILES = Makefile
FILES = $(CFILES) $(HFILES)
CC = clang
OUT = bin/main
FLAGS = -ggdb3 -O3 -lm -o $(OUT)


main: $(FILES)
	$(CC) $(CFILES) $(FLAGS)
