#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cursor.h"
#include "document.h"

const size_t NUM_DOCS = 100;

ic_Document* make_document(uint64_t id) {
    ic_Document* doc = malloc(sizeof(*doc));
    doc->version = 0;
    doc->id = id;
    return doc;
}

int main(int argc, char** argv) {
    printf("welcome to icecore\n");

    ic_Document* documents[NUM_DOCS];
    for (size_t i = 0; i < NUM_DOCS; i++) {
        documents[i] = make_document(i);
    }

    ic_Cursor* cursor = ic_cursor_from_array(documents, NUM_DOCS);

    printf("%p\n", cursor->head);
    for (ic_Document* doc = ic_cursor_peek(cursor); doc != NULL; doc=ic_cursor_next(cursor)) {
        printf("%llu\n", doc->id);
    }

    ic_cursor_delete(cursor);
}