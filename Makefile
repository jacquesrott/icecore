.PHONY: lib
	
COMPILER_OPTIONS = -Wall -Werror

all: link

lib: lib/libcti.so
	
lib/libcti.so: lib/cti.c lib/cti.h
	gcc -c $(COMPILER_OPTIONS) -fpic lib/cti.c && gcc -shared -o lib/libcti.so lib/cti.o

cursor: src/cursor.c
	gcc -c $(COMPILER_OPTIONS) src/cursor.c -o build/cursor.o

main: src/main.c
	gcc -c $(COMPILER_OPTIONS) src/main.c -o build/main.o

link: cursor main
	gcc build/*.o -o icecore
