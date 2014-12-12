#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "document.h"
#include "cursor.h"

#define BITS_SET(x, mask) ((x & mask) == mask)

typedef struct {
    Cursor* a_cursor;
    Cursor* b_cursor;
    MergeType type;
    CompareFunc cmp;
    MergeFunc merge;
} JoinCursorMemo;

typedef struct {
    Document** array;
    Document** next;
    Document** last;
} ArrayCursorMemo;


Cursor* cursor_create(CursorNextFunc next, void* memo) {
    Cursor* cursor = malloc(sizeof(*cursor));
    cursor->next = next;
    cursor->memo = memo;
    cursor->head = NULL;
    cursor_next(cursor);
    return cursor;
}

extern inline Document* cursor_peek(Cursor* cursor);

extern inline Document* cursor_next(Cursor* cursor);

void cursor_delete(Cursor* cursor) {
    free(cursor);
}

bool cursor_done(Cursor* cursor) {
    return cursor->head == NULL;
}

void _merge_next(void* memo, Document** value) {
    JoinCursorMemo* m = (JoinCursorMemo*)memo;
    MergeType type = m->type;

    Document* a = cursor_peek(m->a_cursor);
    Document* b = cursor_peek(m->b_cursor);

    while(a != NULL && b != NULL) {
        int cmp = m->cmp(a, b);
        if (cmp == 0){
            *value = m->merge(a, b);
            cursor_next(m->a_cursor);
            cursor_next(m->b_cursor);
            return;
        }
        else if (cmp > 0){
            if (BITS_SET(type, RIGHT)) {
                *value = m->merge(NULL, b);;
                cursor_next(m->b_cursor);
                return;
            }
            b = cursor_next(m->b_cursor);
        }
        else{
            if (BITS_SET(type, LEFT)) {
                *value = m->merge(a, NULL);;
                cursor_next(m->a_cursor);
                return;
            }
            a = cursor_next(m->a_cursor);
        }
    }

    if (a != NULL && BITS_SET(type, LEFT)) {
        *value = m->merge(a, NULL);;
        cursor_next(m->a_cursor);
        return;
    }
    else if (b != NULL && BITS_SET(type, RIGHT)) {
        *value = m->merge(NULL, b);;
        cursor_next(m->b_cursor);
        return;
    }

    *value = NULL;
    free(memo);

}

Cursor* cursor_join(Cursor* ac, Cursor* bc, MergeType type, CompareFunc cmp, MergeFunc merge) {
    JoinCursorMemo* memo = malloc(sizeof(*memo));
    memo->a_cursor = ac;
    memo->b_cursor = bc;
    memo->type = type;
    memo->cmp = cmp;
    memo->merge = merge;
    return cursor_create(&_merge_next, memo);
}

int64_t cmp_docs(Document* a, Document* b){
    if (a->id == b->id) {
        return a->version - b->version;
    }
    return a->id - b->id;
}

Document* merge_docs(Document* a, Document* b){
    return a == NULL ? b : a;
}

Cursor* cursor_merge(Cursor* ac, Cursor* bc, MergeType type) {
    return cursor_join(ac, bc, type, &cmp_docs, &merge_docs);
}


void _array_next(void* memo, Document** value) {
    ArrayCursorMemo* m = (ArrayCursorMemo*)memo;
    if (m->next <= m->last) {
        *value = *(m->next++);
        return;
    }
    *value = NULL;
    free(memo);
}

Cursor* cursor_from_array(Document** array, size_t n) {
    ArrayCursorMemo* memo = malloc(sizeof(*memo));
    memo->array = array;
    memo->next = array;
    memo->last = array + (n - 1);
    return cursor_create(&_array_next, memo);
}


size_t cursor_to_array(Cursor* cursor, Document** array, size_t n) {
    size_t i = 0;
    for(Document* doc = cursor_peek(cursor); doc != NULL; doc = cursor_next(cursor)) {
        array[i++] = doc;
        if (i == n) {
            break;
        }
    }
    return i;
}

