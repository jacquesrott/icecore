#include <stdio.h>
#include "../mtree.h"
#include "../cursor.h"


void test_mtree(){
    MTree* tree = mtree_create(4, 4, false, NULL);
    for (uintptr_t i = 1; i < 10; i++) {
        mtree_insert(tree, i, (void*)i);
        mtree_dump(tree);
    }
    mtree_dump(tree);
    Document* a[20];
    Cursor* c = mtree_cursor(tree);
    cursor_to_array(c, a, 20);
    char msg[30];
    for (uintptr_t i = 1; i < 10; i++) {
        sprintf(msg, "a[%lu] == %lu", i - 1, i);
        sput_fail_unless(a[i - 1] == (Document*)i, msg);
    }
}


