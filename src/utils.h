#ifndef IC_UTILS_H
#define IC_UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

typedef int (*KeyCmpFunc)(const void*, const void*);


inline int binsearch_left(const intptr_t* keys, int lo, int hi, const intptr_t key) {
    while (lo < hi) {
        int mid = (hi + lo) / 2;
        if (key <= keys[mid]) {
            hi = mid;
        }
        else{
            lo = mid + 1;
        }
    }
    return lo;
}


inline int binsearch_right(const intptr_t* keys, int lo, int hi, const intptr_t key) {
    while (lo < hi) {
        int mid = (hi + lo) / 2;
        if (key < keys[mid]) {
            hi = mid;
        }
        else{
            lo = mid + 1;
        }
    }
    return lo;
}


inline int binsearch_left_cmp(void** keys, int lo, int hi, KeyCmpFunc cmp, const void* key) {
    while (lo < hi) {
        int mid = (hi + lo) / 2;
        int c = cmp(key, keys[mid]);
        if (c <= 0) {
            hi = mid;
        }
        else{
            lo = mid + 1;
        }
    }
    return lo;
}


inline int binsearch_right_cmp(void** keys, int lo, int hi, KeyCmpFunc cmp, const void* key) {
    while (lo < hi) {
        int mid = (hi + lo) / 2;
        int c = cmp(key, keys[mid]);
        if (c < 0) {
            hi = mid;
        }
        else{
            lo = mid + 1;
        }
    }
    return lo;
}


inline int log2i(size_t x) {
    assert(x != 0);
    size_t i = sizeof(size_t) - 1;
    while (!(x & (1 << i))) {
        i--;
    }
    return i;
}


#endif