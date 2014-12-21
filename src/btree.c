#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "cursor.h"
#include "btree.h"
#include "_tree.h"


struct BTreeNode{
    struct _TreeNode;
    unsigned int size;
    BTreeNode* next;
};

struct BTree{
    struct _Tree;
    KeyCmpFunc cmp;
};


#define CHILD(tree, depth, node, i) _TREE_CHILD(tree, depth, node, i, BTreeNode)
#define ROOT(tree) _TREE_ROOT(tree, BTreeNode)
#define TREE(node) _TREE_TREE(node)
#define KEY(node, i) (*(((void**)(((BTreeNode*)node) + 1)) + i))


BTreeNode* create_node(BTree* tree, unsigned int depth) {
    //printf("create_node(depth=%u, order=%zu)\n", depth, _tree_node_get_order((_Tree*)tree, depth));
    BTreeNode* node = (BTreeNode*)_tree_node_create((_Tree*)tree, depth);
    node->size = 0;
    node->next = NULL;
    for (size_t i = 0; i < _tree_node_get_order((_Tree*)tree, depth); i++) {
        KEY(node, i) = NULL;
    }
    return node;
}

void delete_node(BTreeNode* node){
    free(node);
}

int find_index(const BTree* tree, const BTreeNode* node, const void* key) {
    return binsearch_left_cmp(&KEY(node, 0), 0, node->size, tree->cmp, key);
}

int find_interval_index(const BTree* tree, const BTreeNode* node, const void* key) {
    return binsearch_right_cmp(&KEY(node, 0), 1, node->size, tree->cmp, key) - 1;
}

void node_insert(BTree* tree, unsigned int depth, BTreeNode* node, const void* key, const void* value) {
    //printf("node_insert()\n");
    int i = find_index(tree, node, key);
    if (node->size - i != 0) {
        for (int k = node->size; k > i; k--) {
            KEY(node, k) = KEY(node, k - 1);
            CHILD(tree, depth, node, k) = CHILD(tree, depth, node, k - 1);
        }
    }
    node->size++;
    CHILD(tree, depth, node, i) = (BTreeNode*)value;
    KEY(node, i) = (void*)key;
}


BTreeNode* make_sibling(BTree* tree, unsigned int depth, BTreeNode* node) {
    //printf("make_sibling(depth=%u)\n", depth);
    BTreeNode* sibling = (BTreeNode*)_tree_node_create((_Tree*)tree, depth);
    size_t order = _tree_node_get_order((_Tree*)tree, depth);
    sibling->size = node->size - order / 2;
    node->size = order / 2;
    if (node->next != NULL) {
        sibling->next = node->next;
    }
    node->next = sibling;
    //printf("split %p %i/%i\n", node, node->size, sibling->size);
    memcpy(
        &CHILD(tree, depth, sibling, 0), 
        &CHILD(tree, depth, node, node->size),
        sibling->size * sizeof(BTreeNode*)
    );
    memcpy(
        &KEY(sibling, 0),
        &KEY(node, node->size),
        sibling->size * sizeof(void*)
    );
    return sibling;
}

BTreeNode* split_node(BTree* tree, unsigned int depth, BTreeNode* node, BTreeNode* parent, const void* key) {
    //printf("splitting node %p, depth=%u\n", node, depth);
    BTreeNode* sibling = make_sibling(tree, depth, node);
    node_insert(tree, depth + 1, parent, KEY(sibling, 0), sibling);
    if (tree->cmp(key, KEY(sibling, 0)) <= 0) {
        return node;
    }
    else{
        return sibling;
    }
}

BTreeNode* split_root(BTree* tree, const void* key) {
    //printf("split_root()\n");
    BTreeNode* sibling = make_sibling(tree, tree->depth - 1, ROOT(tree));
    BTreeNode* new_root = create_node(tree, tree->depth);
    BTreeNode* old_root = ROOT(tree);
    new_root->size = 2;
    CHILD(tree, tree->depth, new_root, 0) = old_root;
    CHILD(tree, tree->depth, new_root, 1) = sibling;
    KEY(new_root, 0) = KEY(ROOT(tree), 0);
    KEY(new_root, 1) = KEY(sibling, 0);
    ROOT(tree) = new_root;
    tree->depth++;
    if (tree->cmp(key, KEY(sibling, 0)) < 0) {
        return old_root;
    }
    return sibling;
}

void dump_node(BTree* tree, BTreeNode* node, int depth, unsigned int indent) {
    for(int i = 0; i < indent; i++){
        printf("    ");
    }
    if (node == NULL) {
        printf("NULL\n");
        return;
    }
    
    if (depth == 0) {
        printf("<leaf %p size=%i, keys=%p, children=%p> {", node, node->size, &KEY(node, 0), &CHILD(tree, depth, node, 0));
        for (size_t i = 0; i < node->size; i++) {
            printf("%lu: %p, ", (uintptr_t)KEY(node, i), CHILD(tree, depth, node, i));
        }
        printf("}\n");
    }
    else{
        printf("<node %p size=%i> ", node, node->size);
        for (size_t i = 0; i < node->size; i++) {
            printf("%lu|", (uintptr_t)KEY(node, i));
        }
        printf("\n");
        for (size_t i = 0; i < node->size; i++) {
            dump_node(tree, CHILD(tree, depth, node, i), depth - 1, indent + 1);
        }
    }
}

/* public api */

BTree* btree_create(size_t order, size_t leaf_order, KeyCmpFunc cmp) {
    BTree* tree = malloc(sizeof(*tree));
    tree->order = order;
    tree->leaf_order = leaf_order;
    tree->data_size = sizeof(BTreeNode) + order * sizeof(void*);
    tree->leaf_data_size = sizeof(BTreeNode) + leaf_order * sizeof(void*);
    tree->compressed = false;
    //printf("btree_create() data_size=%zu, leaf_data_size=%zu\n", tree->data_size, tree->leaf_data_size);

    tree->cmp = cmp;
    ROOT(tree) = create_node(tree, 0);
    tree->depth = 1;
    return tree;
}

void btree_delete(BTree* tree) {
    delete_node(ROOT(tree));
    free(tree);
}

void btree_dump(BTree* tree) {
    dump_node(tree, ROOT(tree), tree->depth - 1, 0); 
}

void btree_insert(BTree* tree, const void* key, const void* value) {
    //printf("btree_insert()\n");
    BTreeNode* path[tree->depth];
    BTreeNode** end = path + tree->depth;
    BTreeNode** p = end;
    *(--p) = ROOT(tree);

    BTreeNode* node = ROOT(tree);
    for (int i = tree->depth - 1; i > 0 ; i--) {
        int k = find_interval_index(tree, node, key);
        node = CHILD(tree, i, node, k);
        *(--p) = node;
    }
    if ((*p)->size == tree->leaf_order) {
        // have to split the leaf node
        ++p;
        unsigned int depth = 0;
        while (p != end && (*p)->size == tree->order) {
            // walk up the stack until we find a node that doesn't have to be split.
            ++p;
            depth++;
        }
        if (p == end) {
            // all nodes in the path are full. split the root.
            *(--p) = split_root(tree, key);
            depth--;
        }
        while (p != path) {
            // walk down the stack and split nodes.
            BTreeNode** q = p;
            *(--p) = split_node(tree, depth, *(q-1), *q, key);
            depth--;
        }
    }
    node_insert(tree, 0, *p, key, (void*)value);
}

void btree_get(const BTree* tree, const void* key, void** value) {
   BTreeNode* node = ROOT(tree);
   for (int depth = tree->depth - 1; depth > 0; depth--) {
       int k = find_interval_index(tree, node, key);
       node = CHILD(tree, depth, node, k);
       if (node == NULL) {
           *value = NULL;
           return;
       }
   }
   int i = find_index(tree, node, key);
   if (KEY(node, i) != key) {
       *value = NULL;
       return;
   }
   *value = CHILD(tree, 0, node, i);
}


typedef struct {
    const BTree* tree;
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
        *next = (Document*)CHILD(m->tree, 0, m->node, m->i++);
        return;
    }
    *next = NULL;
    free(memo);
}

Cursor* btree_cursor(const BTree* tree) {
    BTreeCursorMemo* memo = malloc(sizeof(*memo));
    BTreeNode* node = ROOT(tree);
    for (int depth = tree->depth - 1; depth > 0; depth--) {
        node = CHILD(tree, depth, node, 0);
    }
    memo->tree = tree;
    memo->node = node;
    memo->i = 0;
    return cursor_create(&_btree_cursor_next, memo);
}

Cursor* btree_range_cursor(const BTree* tree, const void* from_key, const void* to_key) {
   return NULL;
}

