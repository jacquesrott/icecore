#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "mtree.h"
#include "cursor.h"
#include "error.h"
#include "utils.h"
#include "_tree.h"


#define CHILD(tree, depth, node, i) _TREE_CHILD(tree, depth, node, i, MTreeNode)
#define ROOT(tree) _TREE_ROOT(tree, MTreeNode)
#define TREE(node) _TREE_TREE(node)


static inline const MTree* node_get_tree(const MTree* tree, MTreeNode* node) {
    return tree->compressed ? (MTree*)_TREE_TREE(node) : tree;
}

MTreeNode* node_create(MTree* tree, unsigned int depth){
    return (MTreeNode*)_tree_node_create((_Tree*)tree, depth);
}

MTreeNode* node_copy(MTree* tree, MTreeNode* base, unsigned int depth){
    return (MTreeNode*)_tree_node_copy((_Tree*)tree, (_TreeNode*)base, depth);
}

void node_delete(MTree* tree, MTreeNode* node, unsigned int depth) {
    if (node == NULL || node_get_tree(tree, node) != tree) {
        return;
    }
    if (depth != 0) {
        for(unsigned int i = 0; i < tree->order; i++) {
            node_delete(tree, CHILD(tree, depth, node, i), depth - 1);
        }
    }
    free(node);
}

size_t node_size(MTree* tree, MTreeNode* node, unsigned int depth) {
    if(node == NULL || node_get_tree(tree, node) != tree){
        return 0;
    }
    if (depth == 0) {
        return sizeof(MTreeNode) + (tree->leaf_order - 1) * sizeof(void*);
    }
    size_t size = sizeof(*node);
    for(unsigned int i = 0; i < tree->order; i++) {
        size += node_size(tree, CHILD(tree, depth, node, i), depth - 1);
    }
    return size;
}

void node_dump(const MTree* tree, MTreeNode* node, unsigned int depth, unsigned int indent){
    for(int i = 0; i < indent; i++){
        printf("    ");
    }
    if(node == NULL){
        printf("NULL\n");
        return;
    }
    if(depth == 0){
        printf("<%p> ", node);
        for(int i = 0; i < tree->leaf_order; i++){
            void* value = CHILD(tree, depth, node, i);
            if (value == NULL){
                printf("-, ");
            }
            else{
                printf("%lu, ", (uintptr_t)value);
            }
        }
        printf("\n");
    }
    else{
        printf("<%p> tree=%p\n", node, node_get_tree(tree, node));
        for(unsigned int i = 0; i < tree->order; i++) {
            node_dump(tree, CHILD(tree, depth, node, i), depth - 1, indent + 1);
        }
    }
}


MTree* mtree_create(size_t order, size_t leaf_order, bool compressed, MTree* base) {
    assert(compressed || base == NULL);
    //printf("mtree_create()\n");
    MTree* tree = malloc(sizeof(*tree));
    tree->order = order;
    tree->leaf_order = leaf_order;
    tree->compressed = compressed;
    tree->log_order = log2i(order);
    tree->log_leaf_order = log2i(leaf_order);
    tree->data_size = compressed ? sizeof(MTree*) : 0;
    tree->leaf_data_size = tree->data_size;

    assert(tree->order == (1 << tree->log_order));
    assert(tree->leaf_order == (1 << tree->log_leaf_order));
    
    if (base != NULL) {
        ROOT(tree) = node_copy(tree, ROOT(base), 0);
        tree->depth = base->depth;
    }
    else{
        ROOT(tree) = node_create(tree, 0);
        tree->depth = 1;
    }

    return tree;
}


size_t mtree_capacity(MTree* tree){
    return tree->order * (1 << tree->depth);
}


unsigned int get_child_index(MTree* tree, uintptr_t x, int depth) {
    const uintptr_t mask = (1 << tree->log_order) - 1;
    size_t i = tree->log_order * depth;
    return (x & (mask << i)) >> i;
}


void mtree_insert(MTree* tree, uintptr_t key, const void* value) {
    //printf("mtree_insert()\n");
    size_t capacity = mtree_capacity(tree);
    while(key >= capacity){
        MTreeNode* new_root = node_create(tree, tree->depth);
        CHILD(tree, tree->depth, new_root, 0) = ROOT(tree);
        ROOT(tree) = new_root;
        tree->depth += 1;
        capacity *= tree->order;
    }
    
    uintptr_t x = (key - 1) / tree->leaf_order;
    MTreeNode* node = ROOT(tree);

    for(int i = tree->depth - 1; i >= 0; i--) {
        unsigned int child_index = get_child_index(tree, x, i);
        MTreeNode* child = CHILD(tree, i, node, child_index);
        if (child == NULL){
            child = node_create(tree, i);
            CHILD(tree, i, node, child_index) = child;
        }
        if (node_get_tree(tree, child) != tree) {
            child = node_copy(tree, child, i);
            CHILD(tree, i, node, child_index) = child;
        }
        node = child;
    }
    CHILD(tree, 0, node, (key - 1) % tree->leaf_order) = (MTreeNode*)value;
}

ice_t mtree_get(MTree* tree, uintptr_t key, void** value){
    //printf("mtree_get()\n");
    if (key > mtree_capacity(tree)){
        *value = NULL;
        return ICE_DOCUMENT_DOES_NOT_EXIST;
    }
    MTreeNode* node = ROOT(tree);
    uintptr_t x = (key - 1) / tree->leaf_order;
    for(int i = tree->depth - 1; i >= 0; i--) {
        unsigned int child_index = get_child_index(tree, x, i);
        MTreeNode* child = CHILD(tree, i, node, child_index);
        if (child == NULL){
            return ICE_DOCUMENT_DOES_NOT_EXIST;
        }
        node = child;
    }
    *value = CHILD(tree, 0, node, (key - 1) % tree->leaf_order);
    return ICE_OK;
}

void mtree_delete(MTree* tree) {
    node_delete(tree, ROOT(tree), tree->depth);
    free(tree);
}

size_t mtree_size(MTree* tree) {
    return sizeof(*tree) + node_size(tree, ROOT(tree), tree->depth);
}

void mtree_dump(MTree* tree){
    node_dump(tree, ROOT(tree), tree->depth, 0);
}

void mtree_walk(MTree* tree, TreeWalkCallback callback) {
    _tree_walk((_Tree*)tree, callback);
}

Cursor* mtree_cursor(MTree* tree) {
    return _tree_cursor((_Tree*)tree);
}

