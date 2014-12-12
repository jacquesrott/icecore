#ifndef IC_BTREE_H
#define IC_BTREE_H

#include <stdlib.h>

#define BTREE_ORDER 4

typedef struct BTreeNode BTreeNode;
typedef struct BTree BTree;
typedef int (*BTreeKeyCmpFunc)(const void*, const void*);

BTree* btree_create(BTreeKeyCmpFunc);
void btree_delete(BTree*);
void btree_insert(BTree*, const void* key, const void* value);
void btree_get(const BTree*, const void* key, void** value);
void btree_dump(const BTree*);

#endif