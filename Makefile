.PHONY: clean test

<<<<<<< HEAD
COMPILER = clang
COMPILER_OPTIONS = -Wall -Werror -I lib/sput-1.3.0 -std=c11
=======
COMPILER = gcc
COMPILER_OPTIONS = -Wall -Werror -I lib/sput-1.3.0 -std=c11 -fms-extensions -Wno-microsoft
>>>>>>> Unified tree structs
COMPILE = $(COMPILER) -c $(COMPILER_OPTIONS)

LINKER = $(COMPILER)
LINKER_OPTIONS = -lsodium
LINK = $(LINKER) $(COMPILER_OPTIONS) $(LINKER_OPTIONS)


all: bin/icecore

clean:
	rm -rf build && rm -rf bin

build:
	mkdir build

build/%.o: src/%.c build
	$(COMPILE) $< -o $@

bin:
	mkdir bin

<<<<<<< HEAD
bin/icecore: build/main.o build/cursor.o build/versionindex.o build/error.o build/utils.o build/filestore.o build/array.o build/utils.o bin
	$(LINK) build/main.o build/cursor.o build/versionindex.o build/error.o build/array.o build/utils.o -o bin/icecore

bin/test-icecore: build/cursor.o bin src/tests/cursor.c src/tests/main.c build/btree.o build/utils.o build/filestore.o src/tests/filestore.c build/array.o src/tests/array.c
	$(LINK) src/tests/main.c build/cursor.o build/btree.o build/filestore.o build/array.o build/utils.o -o bin/test-icecore
=======
bin/icecore: build/main.o build/cursor.o build/versionindex.o build/error.o build/utils.o build/mtree.o build/_tree.o bin
	$(LINK) build/main.o build/cursor.o build/versionindex.o build/error.o build/utils.o build/mtree.o build/_tree.o -o bin/icecore

bin/test-icecore: build/cursor.o bin src/tests/cursor.c src/tests/main.c build/btree.o build/utils.o build/_tree.o build/mtree.o
	$(LINK) src/tests/main.c build/cursor.o build/btree.o build/utils.o build/mtree.o build/_tree.o -o bin/test-icecore
>>>>>>> Unified tree structs

test: bin/test-icecore
	rm -rf /tmp/test-filestore/
	bin/test-icecore

bin/btree: build/btree.o build/utils.o build/cursor.o bin
	$(LINK) build/btree.o build/utils.o build/cursor.o -o bin/btree
