#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cursor.h"
#include "document.h"
#include "versionindex.h"

const size_t NUM_DOCS = 70;

Document* _make_document(uint64_t id) {
    Document* doc = malloc(sizeof(*doc));
    doc->version = 0;
    doc->id = id;
    return doc;
}

void _print_cursor(Cursor* cursor) {
    for (Document* doc = cursor_peek(cursor); doc != NULL; doc=cursor_next(cursor)) {
        printf("%llu, ", doc->id);
    }
    printf("\n");
}


int main(int argc, char** argv) {
    printf("welcome to icecore\n");

    Document* docs1[NUM_DOCS];
    Document* docs2[NUM_DOCS];
    Document* docs3[NUM_DOCS];

    for (size_t i = 0; i < NUM_DOCS; i++) {
        docs1[i] = _make_document(1 + 1 * i);
        docs2[i] = _make_document(1 + 2 * i);
        docs3[i] = _make_document(1 + 3 * i);
    }

    Cursor* cursor = cursor_from_array(docs2, NUM_DOCS);
    _print_cursor(cursor);
    cursor_delete(cursor);

    Cursor* c2 = cursor_from_array(docs2, NUM_DOCS);
    Cursor* c3 = cursor_from_array(docs3, NUM_DOCS);
    Cursor* c = cursor_merge(c2, c3, INNER);
    _print_cursor(c);
    
    // index test
    DocumentVersionIndex* idx = versionindex_create();
    for(int i = 0; i < NUM_DOCS; i++){
        Document* doc = docs1[i];
        IndexError error = versionindex_insert(idx, doc->id, i+1, doc);
        if (error) {
            printf("index error %i\n", error);
        }
        //tree_dump(idx->trees[i+1]);
    }
    
    for(int i=1;i<NUM_DOCS + 1;i++){
        Document* doc;
        versionindex_get(idx, i, NUM_DOCS/2, &doc);
    }
    tree_dump(idx->trees[20]);
    tree_dump(idx->trees[40]);
    Cursor* dv_cursor = versionindex_cursor(idx, NUM_DOCS/2);
    _print_cursor(dv_cursor);
    cursor_delete(dv_cursor);
    printf("index size = %zu bytes\n", versionindex_size(idx));
    versionindex_delete(idx);
}

