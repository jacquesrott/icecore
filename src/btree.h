#ifndef IC_BTREE_H
#define IC_BTREE_H

#include <stdlib.h>

#define BTREE_ORDER 4

struct BTreeNode;
typedef struct BTreeNode BTreeNode;

struct BTreeNode{
   unsigned int size;
   const void* keys[BTREE_ORDER];
   BTreeNode* children[BTREE_ORDER];
};


typedef int (*BTreeKeyCmpFunc)(const void*, const void*);

typedef struct {
   BTreeKeyCmpFunc cmp;
   BTreeNode* root;
   int depth;
} BTree;


BTree* btree_create(BTreeKeyCmpFunc);
void btree_delete(BTree*);
void btree_insert(BTree*, const void* key, const void* value);
void btree_get(const BTree*, const void* key, void** value);
void btree_dump(const BTree*);

#endif