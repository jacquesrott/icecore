#ifndef IC_BTREE_H
#define IC_BTREE_H

#include <stdlib.h>
#include "cursor.h"
#include "utils.h"

#define BTREE_ORDER 4

typedef struct BTreeNode BTreeNode;
typedef struct BTree BTree;

BTree* btree_create(size_t order, size_t leaf_order, KeyCmpFunc cmp);
void btree_delete(BTree*);
void btree_insert(BTree*, const void* key, const void* value);
void btree_get(const BTree*, const void* key, void** value);
void btree_dump(BTree*);
Cursor* btree_cursor(const BTree*);
Cursor* btree_range_cursor(const BTree*, const void* from_key, const void* to_key);

#endif