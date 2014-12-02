#include "../document.h"
#include "../cursor.h"


ic_Document* make_document(uint64_t id) {
    ic_Document* doc = malloc(sizeof(*doc));
    doc->version = 0;
    doc->id = id;
    return doc;
}


void print_cursor(ic_Cursor* cursor) {
    for (ic_Document* doc = ic_cursor_peek(cursor); doc != NULL; doc=ic_cursor_next(cursor)) {
        printf("%llu, ", doc->id);
    }
    printf("\n");
}


void test_cursor(){
    const size_t NUM_DOCS = 10;

    ic_Document* docs2[NUM_DOCS];
    ic_Document* docs3[NUM_DOCS];

    for (size_t i = 0; i < NUM_DOCS; i++) {
        docs2[i] = make_document(2 * i);
        docs3[i] = make_document(3 * i);
    }

    ic_Cursor* cursor = ic_cursor_from_array(docs2, NUM_DOCS);
    print_cursor(cursor);
    ic_cursor_delete(cursor);

    ic_Cursor* c2 = ic_cursor_from_array(docs2, NUM_DOCS);
    ic_Cursor* c3 = ic_cursor_from_array(docs3, NUM_DOCS);
    ic_Cursor* c = ic_cursor_merge(c2, c3, INNER);
    
    ic_Document* output[4];
    ic_Document* expected[4] = {docs2[0], docs2[3], docs2[6], docs2[9]};

    size_t len = ic_cursor_to_array(c, output, 4);
    sput_fail_unless(len == 4, "merge of [0,2,4,6,8,12,14,16,18] and [0,3,6,9,12,15,18,21,24,27] has length 4");
    sput_fail_if(memcmp(output, expected, sizeof(expected)), "merge equals [0,6,12,18]");
    //print_cursor(c);

    //sput_fail_unless(ass, msg);
}


