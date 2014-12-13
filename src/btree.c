#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cursor.h"
#include "btree.h"


struct BTreeNode{
   unsigned int size;
   const void* keys[BTREE_ORDER];
   BTreeNode* children[BTREE_ORDER];
   BTreeNode* next;
};

struct BTree{
   BTreeKeyCmpFunc cmp;
   BTreeNode* root;
   int depth;
};


BTreeNode* create_node() {
   BTreeNode* node = malloc(sizeof(*node));
   node->size = 0;
   node->next = NULL;
   for (size_t i = 0; i < BTREE_ORDER; i++) {
       node->children[i] = NULL;
       node->keys[i] = NULL;
   }
   return node;
}

void delete_node(BTreeNode* node){
    free(node);
}

int binsearch_left(const void*const* keys, int lo, int hi, BTreeKeyCmpFunc cmp, const void* key) {
    while (lo < hi) {
        int mid = (hi + lo) / 2;
        int c = cmp(key, keys[mid]);
        if (c <= 0) {
            hi = mid;
        }
        else{
            lo = mid + 1;
        }
    }
    return lo;
}

int binsearch_right(const void*const* keys, int lo, int hi, BTreeKeyCmpFunc cmp, const void* key) {
    while (lo < hi) {
        int mid = (hi + lo) / 2;
        int c = cmp(key, keys[mid]);
        if (c < 0) {
            hi = mid;
        }
        else{
            lo = mid + 1;
        }
    }
    return lo;
}

int find_index(const BTree* tree, const BTreeNode* node, const void* key) {
    return binsearch_left(node->keys, 0, node->size, tree->cmp, key);
}

int find_interval_index(const BTree* tree, const BTreeNode* node, const void* key) {
    return binsearch_right(node->keys, 1, node->size, tree->cmp, key) - 1;
}

void node_insert(BTree* tree, BTreeNode* node, const void* key, const void* value) {
    int i = find_index(tree, node, key);
    if (node->size - i != 0) {
        for (int k = node->size; k > i; k--) {
            node->keys[k] = node->keys[k - 1];
            node->children[k] = node->children[k - 1];
        }
    }
    node->size++;
    node->children[i] = (BTreeNode*)value;
    node->keys[i] = key;
}


BTreeNode* make_sibling(BTreeNode* node) {
    BTreeNode* sibling = create_node();
    sibling->size = node->size - BTREE_ORDER / 2;
    node->size = BTREE_ORDER / 2;
    if (node->next != NULL) {
        sibling->next = node->next;
    }
    node->next = sibling;
    //printf("split %p %i/%i\n", node, node->size, sibling->size);
    memcpy(sibling->children, node->children + node->size, sibling->size * sizeof(BTreeNode*));
    memcpy(sibling->keys, node->keys + node->size, sibling->size * sizeof(void*));
    return sibling;
}

BTreeNode* split_node(BTree* tree, BTreeNode* node, BTreeNode* parent, const void* key) {
    BTreeNode* sibling = make_sibling(node);
    //printf("insert new sibling %p into %p (key=%lu)\n", sibling, parent, (uintptr_t)(sibling->keys[0]));
    node_insert(tree, parent, sibling->keys[0], sibling);
    if (tree->cmp(key, sibling->keys[0]) <= 0) {
        return node;
    }
    else{
        return sibling;
    }
}

BTreeNode* split_root(BTree* tree, const void* key) {
    BTreeNode* sibling = make_sibling(tree->root);
    BTreeNode* new_root = create_node();
    BTreeNode* old_root = tree->root;
    new_root->size = 2;
    new_root->children[0] = old_root;
    new_root->children[1] = sibling;
    new_root->keys[0] = tree->root->keys[0];
    new_root->keys[1] = sibling->keys[0];
    tree->root = new_root;
    tree->depth++;
    if (tree->cmp(key, sibling->keys[0]) < 0) {
        return old_root;
    }
    return sibling;
}

void dump_node(BTreeNode* node, int depth, unsigned int indent) {
    for(int i = 0; i < indent; i++){
        printf("    ");
    }
    if (node == NULL) {
        printf("NULL\n");
        return;
    }
    
    if (depth == 1) {
        printf("<leaf %p size=%i> {", node, node->size);
        for (size_t i = 0; i < node->size; i++) {
            printf("%lu: %p, ", (uintptr_t)node->keys[i], node->children[i]);
        }
        printf("}\n");
    }
    else{
        printf("<node %p size=%i> ", node, node->size);
        for (size_t i = 0; i < node->size; i++) {
            printf("%lu|", (uintptr_t)node->keys[i]);
        }
        printf("\n");
        for (size_t i = 0; i < node->size; i++) {
            dump_node(node->children[i], depth - 1, indent + 1);
        }
    }
}

/* public api */

BTree* btree_create(BTreeKeyCmpFunc cmp) {
   BTree* tree = malloc(sizeof(*tree));
   tree->cmp = cmp;
   tree->root = create_node();
   tree->depth = 1;
   return tree;
}

void btree_delete(BTree* tree) {
    delete_node(tree->root);
    free(tree);
}

void btree_dump(const BTree* tree) {
    dump_node(tree->root, tree->depth, 0); 
}

void btree_insert(BTree* tree, const void* key, const void* value) {
    BTreeNode* path[tree->depth];
    BTreeNode** end = path + tree->depth;
    BTreeNode** p = end;
    *(--p) = tree->root;

    BTreeNode* node = tree->root;
    for (int i = tree->depth - 2; i >= 0 ; i--) {
        int k = find_interval_index(tree, node, key);
        node = node->children[k];
        *(--p) = node;
    }
    while (p != end && (*p)->size == BTREE_ORDER) {
        // walk up the stack until we find a node that doesn't have to be split.
        ++p;
    }
    if (p == end) {
        // all nodes in the path are full. split the root.
        *(--p) = split_root(tree, key);
    }
    while (p != path) {
        // walk down the stack and split nodes.
        BTreeNode** q = p;
        *(--p) = split_node(tree, *(q-1), *q, key);
    }
    node_insert(tree, *p, key, (void*)value);
}

void btree_get(const BTree* tree, const void* key, void** value) {
   BTreeNode* node = tree->root;
   for (int depth = tree->depth - 1; depth > 0; depth--) {
       int k = find_interval_index(tree, node, key);
       node = node->children[k];
       if (node == NULL) {
           *value = NULL;
           return;
       }
   }
   int i = find_index(tree, node, key);
   if (node->keys[i] != key) {
       *value = NULL;
       return;
   }
   *value = node->children[i];
}


typedef struct {
    BTreeNode* node;
    size_t i;
} BTreeCursorMemo;


void _btree_cursor_next(void* memo, Document** next) {
    BTreeCursorMemo* m = (BTreeCursorMemo*)memo;
    if (m->i >= m->node->size) {
        m->node = m->node->next;
        m->i = 0;
    }
    if (m->node != NULL) {
        *next = (Document*)m->node->children[m->i++];
        return;
    }
    *next = NULL;
    free(memo);
}

Cursor* btree_cursor(const BTree* tree) {
    BTreeCursorMemo* memo = malloc(sizeof(*memo));
    BTreeNode* node = tree->root;
    for (int depth = tree->depth - 1; depth > 0; depth--) {
        node = node->children[0];
    }
    memo->node = node;
    memo->i = 0;
    return cursor_create(&_btree_cursor_next, memo);
}

Cursor* btree_range_cursor(const BTree* tree, const void* from_key, const void* to_key) {
   return NULL;
}

