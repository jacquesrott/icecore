#ifndef INDEX_H
#define INDEX_H

#include <stdint.h>
#include <string.h>

typedef uint64_t Value;
typedef uint64_t Version;
typedef uint64_t Id;

const Value NONE = 0;
const unsigned int INDEX_NODE_BITS = 2;

const unsigned int INDEX_NODE_SIZE = 1 << INDEX_NODE_BITS;
const Version INDEX_NODE_MASK = INDEX_NODE_SIZE - 1;


typedef enum {
    OK = 0,
    DOCUMENT_DOES_NOT_EXIST,
    VERSION_DOES_NOT_EXIST,
    VERSION_SKIPPED,
} IndexError;


typedef struct IndexNode{
    Version version;
    struct IndexNode* children[INDEX_NODE_SIZE];
} IndexNode;

typedef struct{
    Version version;
    IndexNode* root;
    unsigned int depth;
} Tree;


typedef struct {
    Version last_version;
    Tree** trees;
} Index;


extern Tree* tree_create(Version version, Tree* base);
extern size_t tree_capacity(Tree* tree);
extern void tree_insert(Tree* tree, Id id, Value value);
extern IndexError tree_get(Tree* tree, Id id, Value* value);
extern void tree_delete(Tree* tree);
extern size_t tree_size(Tree* tree);
extern void tree_dump(Tree* tree);

extern Index* index_create();
extern IndexError index_insert(Index* idx, Id id, Version version, Value value);
extern IndexError index_get(Index* idx, Id id, Version version, Value* value);
extern size_t index_size(Index* idx);
extern void index_delete(Index* idx);
extern void index_walk(Index* idx, Version version, void (*callback)(Value value));

#endif