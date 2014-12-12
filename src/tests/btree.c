#include <stdio.h>
#include "../btree.h"

int btree_test_intcmp(const void* a, const void* b){
    return (int)((intptr_t)a - (intptr_t)b);
}

void test_btree(){
    BTree* tree = btree_create(&btree_test_intcmp);
    for (intptr_t i = 0; i < 10; i++) {
        intptr_t x = 4 * i + 1;
        btree_insert(tree, (void*)x, (void*)x);
    }
    for (intptr_t i = 0; i < 10; i++) {
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
    for (intptr_t i = 1; i < 40; i++) {
        btree_get(tree, (void*)i, &value);
        sprintf(msg, "tree[%li] == %li", i, i);
        sput_fail_unless(value == (void*)i, msg);
    }
}


