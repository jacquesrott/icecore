#ifndef IC_MTREE_H
#define IC_MTREE_H

#include <stdint.h>
#include <string.h>

#include "error.h"
#include "cursor.h"
#include "_tree.h"


typedef struct MTreeNode{
    struct _TreeNode;
} MTreeNode;


typedef struct MTree{
    struct _Tree;
    size_t log_order;
    size_t log_leaf_order;
} MTree;


typedef struct CompressedMTreeNode{
    struct MTreeNode;
    MTree* tree;
} CompressedMTreeNode;



MTree* mtree_create(size_t order, size_t leaf_order, bool compressed, MTree* base);
void mtree_insert(MTree* tree, uintptr_t key, const void* value);
ice_t mtree_get(MTree* tree, uintptr_t key, void** value);
void mtree_delete(MTree* tree);
size_t mtree_size(MTree* tree);
void mtree_dump(MTree* tree);
Cursor* mtree_cursor(MTree* tree);
void mtree_walk(MTree* tree, TreeWalkCallback callback);

#endif