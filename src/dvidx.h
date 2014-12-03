#ifndef IC_DVIDX_H
#define IC_DVIDX_H

#include <stdint.h>
#include <string.h>

#include "document.h"
#include "cursor.h"


#define INDEX_NODE_BITS 1
#define INDEX_NODE_SIZE (1 << INDEX_NODE_BITS)


typedef enum {
    OK = 0,
    DOCUMENT_DOES_NOT_EXIST,
    VERSION_DOES_NOT_EXIST,
    VERSION_SKIPPED,
} ic_IndexError;


typedef struct IndexNode{
    ic_Version version;
    struct IndexNode* children[INDEX_NODE_SIZE];
} IndexNode;

typedef struct{
    ic_Version version;
    IndexNode* root;
    unsigned int depth;
} Tree;


typedef struct {
    ic_Version last_version;
    Tree** trees;
} ic_DocumentVersionIndex;


Tree* tree_create(ic_Version version, Tree* base);
size_t tree_capacity(Tree* tree);
void tree_insert(Tree* tree, ic_Id id, ic_Document* value);
ic_IndexError tree_get(Tree* tree, ic_Id id, ic_Document** value);
void tree_delete(Tree* tree);
size_t tree_size(Tree* tree);
void tree_dump(Tree* tree);

ic_DocumentVersionIndex* ic_dvidx_create();
ic_IndexError ic_dvidx_insert(ic_DocumentVersionIndex* idx, ic_Id id, ic_Version version, ic_Document* value);
ic_IndexError ic_dvidx_get(ic_DocumentVersionIndex* idx, ic_Id id, ic_Version version, ic_Document** value);
size_t ic_dvidx_size(ic_DocumentVersionIndex* idx);
void ic_dvidx_delete(ic_DocumentVersionIndex* idx);
ic_Cursor* ic_dvidx_cursor(ic_DocumentVersionIndex* idx, ic_Version version);

#endif