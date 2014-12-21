#ifndef IC_TREE_H
#define IC_TREE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "cursor.h"


typedef struct _TreeNode{
} _TreeNode;

typedef struct _Tree{
    struct _TreeNode* root;
    unsigned int depth;
    bool compressed;
    size_t order;
    size_t leaf_order;
    size_t data_size;
    size_t leaf_data_size;
} _Tree;

typedef struct _CompressedTreeNode{
    struct _TreeNode;
    _Tree* tree;
} _CompressedTreeNode;

typedef void (*TreeWalkCallback)(void*);

#define _TREE_CHILD(tree, depth, node, i, T) (*(((T**)(((char*)node) + (depth == 0 ? tree->leaf_data_size : tree->data_size))) + i))
#define _TREE_ROOT(tree, T) (*(T**)&tree->root)
#define _TREE_TREE(node) ((_CompressedTreeNode*)node)->tree


static inline size_t _tree_node_get_order(_Tree* tree, unsigned int depth) {
    return depth == 0 ? ((_Tree*)tree)->leaf_order : ((_Tree*)tree)->order;
}

static inline size_t _tree_node_raw_size(_Tree* tree, unsigned int depth) {
    size_t size = depth == 0 ? tree->leaf_data_size : tree->data_size;
    return size + sizeof(void*) * _tree_node_get_order(tree, depth);
}


void _tree_walk(_Tree* tree, TreeWalkCallback callback);
Cursor* _tree_cursor(_Tree* tree);


static inline _TreeNode* _tree_node_create(_Tree* tree, unsigned int depth){
    //printf("node_create()\n");
    _TreeNode* node = malloc(_tree_node_raw_size(tree, depth));
    if (tree->compressed) {
        _TREE_TREE(node) = tree;
    }
    //printf("create node %zu %zu %zu\n", sizeof(MTreeNode), tree->order, _raw_node_size(tree, 0));
    unsigned int order = _tree_node_get_order(tree, depth);
    for(unsigned int i = 0; i < order; i++) {
        _TREE_CHILD(tree, depth, node, i, _TreeNode) = NULL;
    }
    return node;
}

static inline _TreeNode* _tree_node_copy(_Tree* tree, _TreeNode* base, unsigned int depth){
    //printf("node_copy()\n");
    _TreeNode* node = malloc(_tree_node_raw_size(tree, depth));
    if (tree->compressed) {
        _TREE_TREE(node) = tree;
    }
    memcpy(
        &_TREE_CHILD(tree, depth, node, 0, _TreeNode),
        &_TREE_CHILD(tree, depth, base, 0, _TreeNode),
        sizeof(_TreeNode*) * _tree_node_get_order(tree, depth)
    );
    return node;
}


#endif
