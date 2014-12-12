#ifndef CURSOR_H
#define CURSOR_H

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "document.h"


typedef void (*CursorNextFunc)(void*, Document**);
typedef int64_t (*CompareFunc)(Document*, Document*);
typedef Document* (*MergeFunc)(Document*, Document*);

typedef uint64_t CursorKey;

typedef enum {
    NONE = 0,
    INNER = 1,
    LEFT = 3, // 2 | INNER
    RIGHT = 5, // 4 | INNER
    OUTER = 7, // LEFT | RIGHT | INNER
} MergeType;


typedef struct {
    void* memo;
    Document* head;
    CursorNextFunc next;
} Cursor;

Cursor* cursor_create(CursorNextFunc next, void* memo);
Cursor* cursor_from_array(Document** array, size_t n);
void cursor_delete(Cursor* cursor);
bool cursor_done(Cursor* cursor);
Cursor* cursor_join(Cursor* ac, Cursor* bc, MergeType type, CompareFunc cmp, MergeFunc merge);
Cursor* cursor_merge(Cursor* ac, Cursor* bc, MergeType type);
size_t cursor_to_array(Cursor* cursor, Document** array, size_t n);

inline Document* cursor_peek(Cursor* cursor) {
    return cursor->head;
}

inline Document* cursor_next(Cursor* cursor) {
    cursor->next(cursor->memo, &cursor->head);
    return cursor->head;
}

#endif