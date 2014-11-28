#ifndef INDEX_H
#define INDEX_H

#include <stdint.h>

typedef uint64_t Value;
typedef uint64_t Version;
typedef uint64_t Id;

const Value NONE = 0;
const unsigned int LEAF_NODE_SIZE = 16;
const unsigned int MAX_VERSIONS = 1000;

typedef enum {
    OK = 0,
    DOCUMENT_DOES_NOT_EXIST,
    VERSION_DOES_NOT_EXIST,
    VERSION_SKIPPED,
} IndexError;


typedef struct{
    Version version;
    Value values[LEAF_NODE_SIZE];
} LeafNode;


typedef struct IndexNode{
    Version version;
    struct IndexNode* children[2];
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
extern int tree_get(Tree* tree, Id id, Value* value);
extern void tree_delete(Tree* tree);
extern size_t tree_size(Tree* tree);
extern void tree_dump(Tree* tree);


#endif