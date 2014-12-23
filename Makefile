.PHONY: clean test

COMPILER = clang
COMPILER_OPTIONS = -Wall -Werror -I lib/sput-1.3.0 -std=c11 -fms-extensions -Wno-microsoft
COMPILE = $(COMPILER) -c $(COMPILER_OPTIONS)

LINKER = $(COMPILER)
LINKER_OPTIONS = -lsodium -ljansson
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

bin/icecore: build/main.o build/cursor.o build/versionindex.o build/mtree.o build/error.o build/utils.o build/filestore.o build/_tree.o build/array.o build/utils.o build/icecore.o build/filestore.o build/md5.o build/cli.o bin
	$(LINK) build/main.o build/cursor.o build/versionindex.o build/mtree.o build/error.o build/array.o build/utils.o build/_tree.o build/icecore.o build/filestore.o build/md5.o build/cli.o -o bin/icecore

bin/test-icecore: build/cursor.o bin src/tests/cursor.c src/tests/main.c build/btree.o build/utils.o build/filestore.o src/tests/filestore.c build/array.o src/tests/array.c build/mtree.o build/_tree.o
	$(LINK) src/tests/main.c build/cursor.o build/btree.o build/filestore.o build/array.o build/utils.o build/mtree.o build/_tree.o -o bin/test-icecore

test: bin/test-icecore
	rm -rf /tmp/test-filestore/
	bin/test-icecore
