.PHONY: lib

all: lib

lib: lib/libcti.so
	
lib/libcti.so:
		cd lib && gcc -c -Wall -Werror -fpic cti.c && gcc -shared -o libcti.so cti.o
