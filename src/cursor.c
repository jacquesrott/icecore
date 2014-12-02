#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "document.h"
#include "cursor.h"

#define BITS_SET(x, mask) ((x & mask) == mask)

typedef struct {
    ic_Cursor* a_cursor;
    ic_Cursor* b_cursor;
    ic_MergeType type;
    ic_CompareFunc cmp;
    ic_MergeFunc merge;
} JoinCursorMemo;

typedef struct {
    ic_Document** array;
    ic_Document** next;
    ic_Document** last;
} ArrayCursorMemo;


ic_Cursor* ic_cursor_create(ic_CursorNextFunc next, void* memo) {
    ic_Cursor* cursor = malloc(sizeof(*cursor));
    cursor->next = next;
    cursor->memo = memo;
    cursor->head = NULL;
    ic_cursor_next(cursor);
    return cursor;
}


void _array_next(void* memo, ic_Document** value) {
    ArrayCursorMemo* m = (ArrayCursorMemo*)memo;
    if (m->next != m->last) {
        *value = *(m->next++);
        return;
    }
    *value = NULL;
    free(memo);
}


ic_Cursor* ic_cursor_from_array(ic_Document** array, size_t n) {
    ArrayCursorMemo* memo = malloc(sizeof(*memo));
    memo->array = array;
    memo->next = NULL;
    memo->last = array + (n - 1);
    return ic_cursor_create(&_array_next, memo);
}

extern inline ic_Document* ic_cursor_peek(ic_Cursor* cursor);

extern inline ic_Document* ic_cursor_next(ic_Cursor* cursor);

void ic_cursor_delete(ic_Cursor* cursor) {
    free(cursor);
}

bool ic_cursor_done(ic_Cursor* cursor) {
    return cursor->head == NULL;
}

void _merge_next(void* memo, ic_Document** value) {
    JoinCursorMemo* m = (JoinCursorMemo*)memo;
    ic_MergeType type = m->type;

    ic_Document* a = ic_cursor_peek(m->a_cursor);
    ic_Document* b = ic_cursor_peek(m->b_cursor);

    while(a != NULL && b != NULL) {
        int cmp = m->cmp(a, b);
        if (cmp == 0){
            *value = m->merge(a, b);
            a = ic_cursor_next(m->a_cursor);
            b = ic_cursor_next(m->b_cursor);
            return;
        }
        else if (cmp > 0){
            if (BITS_SET(type, RIGHT)) {
                *value = m->merge(NULL, b);
            }
            b = ic_cursor_next(m->a_cursor);
            return;
        }
        else{
            if (BITS_SET(type, LEFT)) {
                *value = m->merge(a, NULL);
            }
            a = ic_cursor_next(m->a_cursor);
            return;
        }
    }
    if (a != NULL) {
        *value = a;
        ic_cursor_next(m->a_cursor);
        return;
    }
    else if (b != NULL) {
        *value = b;
        ic_cursor_next(m->b_cursor);
        return;
    }
    *value = NULL;
    free(memo);
}

ic_Cursor* ic_cursor_join(ic_Cursor* ac, ic_Cursor* bc, ic_MergeType type, ic_CompareFunc cmp, ic_MergeFunc merge) {
    JoinCursorMemo* memo = malloc(sizeof(*memo));
    memo->a_cursor = ac;
    memo->b_cursor = bc;
    memo->type = type;
    memo->cmp = cmp;
    memo->merge = merge;
    return ic_cursor_create(&_merge_next, memo);
}

int64_t cmp_docs(ic_Document* a, ic_Document* b){
    if (a->id == b->id) {
        return a->version - b->version;
    }
    return a->id - b->id;
}

ic_Document* merge_docs(ic_Document* a, ic_Document* b){
    return a == NULL ? b : a;
}

ic_Cursor* ic_cursor_merge(ic_Cursor* ac, ic_Cursor* bc, ic_MergeType type) {
    return ic_cursor_join(ac, bc, type, &cmp_docs, &merge_docs);
}

