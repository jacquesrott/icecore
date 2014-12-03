#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cursor.h"
#include "document.h"

const size_t NUM_DOCS = 10;

ic_Document* _make_document(uint64_t id) {
    ic_Document* doc = malloc(sizeof(*doc));
    doc->version = 0;
    doc->id = id;
    return doc;
}

void _print_cursor(ic_Cursor* cursor) {
    for (ic_Document* doc = ic_cursor_peek(cursor); doc != NULL; doc=ic_cursor_next(cursor)) {
        printf("%llu, ", doc->id);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    printf("welcome to icecore\n");

    ic_Document* docs2[NUM_DOCS];
    ic_Document* docs3[NUM_DOCS];

    for (size_t i = 0; i < NUM_DOCS; i++) {
        docs2[i] = _make_document(2 * i);
        docs3[i] = _make_document(3 * i);
    }

    ic_Cursor* cursor = ic_cursor_from_array(docs2, NUM_DOCS);
    _print_cursor(cursor);
    ic_cursor_delete(cursor);

    ic_Cursor* c2 = ic_cursor_from_array(docs2, NUM_DOCS);
    ic_Cursor* c3 = ic_cursor_from_array(docs3, NUM_DOCS);
    ic_Cursor* c = ic_cursor_merge(c2, c3, INNER);
    _print_cursor(c);

}
