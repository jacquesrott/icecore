.PHONY: clean test

COMPILER = gcc
COMPILER_OPTIONS = -Wall -Werror -I lib/sput-1.3.0 -std=c99
COMPILE = $(COMPILER) -c $(COMPILER_OPTIONS)
LINKER = $(COMPILER)
LINKER_OPTIONS = $(COMPILER_OPTIONS)
LINK = $(LINKER) $(LINKER_OPTIONS)


all: bin/icecore

clean:
	rm -rf build && rm -rf bin

build:
	mkdir build

build/%.o: src/%.c build
	$(COMPILE) $< -o $@

bin:
	mkdir bin

bin/icecore: build/main.o build/cursor.o build/versionindex.o bin
	$(LINK) build/main.o build/cursor.o build/versionindex.o -o bin/icecore

bin/test-icecore: build/cursor.o bin src/tests/cursor.c src/tests/main.c build/btree.o
	$(LINK) src/tests/main.c build/cursor.o build/btree.o -o bin/test-icecore

test: bin/test-icecore
	bin/test-icecore

bin/btree: build/btree.o bin
	$(LINK) build/btree.o -o bin/btree
