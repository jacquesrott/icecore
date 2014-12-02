#ifndef IC_CURSOR_H
#define IC_CURSOR_H

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "document.h"


typedef void (*ic_CursorNextFunc)(void*, ic_Document**);
typedef int64_t (*ic_CompareFunc)(ic_Document*, ic_Document*);
typedef ic_Document* (*ic_MergeFunc)(ic_Document*, ic_Document*);

typedef uint64_t ic_CursorKey;

typedef enum {
    NONE = 0,
    INNER = 1,
    LEFT = 3, // 2 | INNER
    RIGHT = 5, // 4 | INNER
    OUTER = 7, // LEFT | RIGHT | INNER
} ic_MergeType;


typedef struct {
    void* memo;
    ic_Document* head;
    ic_CursorNextFunc next;
} ic_Cursor;

ic_Cursor* ic_cursor_create(ic_CursorNextFunc next, void* memo);
ic_Cursor* ic_cursor_from_array(ic_Document** array, size_t n);
void ic_cursor_delete(ic_Cursor* cursor);
bool ic_cursor_done(ic_Cursor* cursor);
ic_Cursor* ic_cursor_join(ic_Cursor* ac, ic_Cursor* bc, ic_MergeType type, ic_CompareFunc cmp, ic_MergeFunc merge);
ic_Cursor* ic_cursor_merge(ic_Cursor* ac, ic_Cursor* bc, ic_MergeType type);

inline ic_Document* ic_cursor_peek(ic_Cursor* cursor) {
    return cursor->head;
}

inline ic_Document* ic_cursor_next(ic_Cursor* cursor) {
    assert(cursor->head != NULL);
    cursor->next(cursor->memo, &cursor->head);
    return cursor->head;
}

#endif