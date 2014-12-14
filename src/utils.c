#include "utils.h"

extern inline int binsearch_left_cmp(const void*const* keys, int lo, int hi, KeyCmpFunc cmp, const void* key);
extern inline int binsearch_right_cmp(const void*const* keys, int lo, int hi, KeyCmpFunc cmp, const void* key);
extern inline int binsearch_left(const intptr_t* keys, int lo, int hi, const intptr_t key);
extern inline int binsearch_right(const intptr_t* keys, int lo, int hi, const intptr_t key);
