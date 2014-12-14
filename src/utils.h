#ifndef IC_UTILS_H
#define IC_UTILS_H

#include <stdint.h>


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


inline int binsearch_left_cmp(const void*const* keys, int lo, int hi, KeyCmpFunc cmp, const void* key) {
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


inline int binsearch_right_cmp(const void*const* keys, int lo, int hi, KeyCmpFunc cmp, const void* key) {
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


#endif