#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cursor.h"
#include "document.h"
#include "dvidx.h"

const size_t NUM_DOCS = 70;

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

    ic_Document* docs1[NUM_DOCS];
    ic_Document* docs2[NUM_DOCS];
    ic_Document* docs3[NUM_DOCS];

    for (size_t i = 0; i < NUM_DOCS; i++) {
        docs1[i] = _make_document(1 + 1 * i);
        docs2[i] = _make_document(1 + 2 * i);
        docs3[i] = _make_document(1 + 3 * i);
    }

    ic_Cursor* cursor = ic_cursor_from_array(docs2, NUM_DOCS);
    _print_cursor(cursor);
    ic_cursor_delete(cursor);

    ic_Cursor* c2 = ic_cursor_from_array(docs2, NUM_DOCS);
    ic_Cursor* c3 = ic_cursor_from_array(docs3, NUM_DOCS);
    ic_Cursor* c = ic_cursor_merge(c2, c3, INNER);
    _print_cursor(c);
    
    // index test
    ic_DocumentVersionIndex* idx = ic_dvidx_create();
    for(int i = 0; i < NUM_DOCS; i++){
        ic_Document* doc = docs1[i];
        ic_IndexError error = ic_dvidx_insert(idx, doc->id, i+1, doc);
        if (error) {
            printf("index error %i\n", error);
        }
        //tree_dump(idx->trees[i+1]);
    }
    
    for(int i=1;i<NUM_DOCS + 1;i++){
        ic_Document* doc;
        ic_dvidx_get(idx, i, NUM_DOCS/2, &doc);
    }
    tree_dump(idx->trees[20]);
    tree_dump(idx->trees[40]);
    ic_Cursor* dv_cursor = ic_dvidx_cursor(idx, NUM_DOCS/2);
    _print_cursor(dv_cursor);
    ic_cursor_delete(dv_cursor);
    printf("index size = %zu bytes\n", ic_dvidx_size(idx));
    ic_dvidx_delete(idx);
}

