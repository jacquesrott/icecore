.PHONY: clean
	
COMPILER = gcc
COMPILER_OPTIONS = -Wall -Werror
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

bin/icecore: build/main.o build/cursor.o bin
	$(LINK) build/main.o build/cursor.o -o bin/icecore
