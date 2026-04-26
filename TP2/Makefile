CC     = gcc
CFLAGS = -Wall -fPIC -g
NASM   = nasm

all: libgini.so programa

gini_asm.o: gini_asm.asm
	$(NASM) -f elf64 -g -F dwarf gini_asm.asm -o gini_asm.o

gini.o: gini.c gini.h
	$(CC) $(CFLAGS) -c -o gini.o gini.c

libgini.so: gini.c gini.h gini_asm.o
	$(CC) $(CFLAGS) -shared -o libgini.so gini.c gini_asm.o

programa: main.c gini.o gini_asm.o
	$(CC) -Wall -g -O0 -o programa main.c gini.o gini_asm.o

run: libgini.so
	python3 main.py

gdb: programa
	gdb ./programa

clean:
	rm -f libgini.so gini_asm.o gini.o programa
