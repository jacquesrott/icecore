#include <stdio.h>

#include "../btree.h"
#include "../cursor.h"


int btree_test_intcmp(const void* a, const void* b){
    return (int)((intptr_t)a - (intptr_t)b);
}

void test_btree(){
    BTree* tree = btree_create(2, 4, &btree_test_intcmp);
    for (intptr_t i = 0; i < 10; i++) {
        intptr_t x = 4 * i + 1;
        btree_insert(tree, (void*)x, (void*)x);
    }
    for (intptr_t i = 1; i < 10; i++) {
        intptr_t x = 4 * i;
        btree_insert(tree, (void*)x, (void*)x);
    }
    for (intptr_t i = 0; i < 10; i++) {
        intptr_t x = 4 * i + 2;
        btree_insert(tree, (void*)x, (void*)x);
    }
    for (intptr_t i = 0; i < 10; i++) {
        intptr_t x = 4 * i + 3;
        btree_insert(tree, (void*)x, (void*)x);
    }
    btree_dump(tree);
    void* value;
    char msg[30];
    for (intptr_t i = 1; i < 4; i++) {
        btree_get(tree, (void*)i, &value);
        sprintf(msg, "tree[%li] == %li", i, i);
        sput_fail_unless(value == (void*)i, msg);
    }
}

void test_btree_cursor() {
    BTree* tree = btree_create(4, 4, &btree_test_intcmp);
    for (intptr_t i = 1; i <= 30; i++) {
        btree_insert(tree, (void*)i, (void*)i);
    }
    Cursor* cursor = btree_cursor(tree);
    Document* out[40];
    char msg[30];
    size_t n = cursor_to_array(cursor, out, 40);
    sput_fail_unless(n == 30, "btree contains 30 elements");
    for (intptr_t i = 0; i < n; i++) {
        sprintf(msg, "out[%li] == %li", i, i);
        sput_fail_unless(out[i] == (Document*)(i + 1), msg);
    }
}


