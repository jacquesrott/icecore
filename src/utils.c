#include <stdio.h>
#include "utils.h"

extern inline int binsearch_left_cmp(void** keys, int lo, int hi, KeyCmpFunc cmp, const void* key);
extern inline int binsearch_right_cmp(void** keys, int lo, int hi, KeyCmpFunc cmp, const void* key);
extern inline int binsearch_left(const intptr_t* keys, int lo, int hi, const intptr_t key);
extern inline int binsearch_right(const intptr_t* keys, int lo, int hi, const intptr_t key);
extern inline int log2i(size_t x);

long get_file_size(FILE *file) {
    long file_size;
    long offset = ftell(file);
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, offset, SEEK_SET);
    return file_size;
}
