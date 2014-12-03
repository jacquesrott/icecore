#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "dvidx.h"
#include "document.h"
#include "cursor.h"

const unsigned int LEAF_NODE_SIZE = 16;
const unsigned int MAX_VERSIONS = 1000;

const ic_Version INDEX_NODE_MASK = INDEX_NODE_SIZE - 1;


typedef struct{
    ic_Version version;
    ic_Document* values[LEAF_NODE_SIZE];
} LeafNode;


LeafNode* leafnode_create(ic_Version version){
    LeafNode* leaf = malloc(sizeof(*leaf));
    leaf->version = version;
    for (unsigned int i = 0; i < LEAF_NODE_SIZE; i++) {
        leaf->values[i] = NULL;
    }
    return leaf;
}

LeafNode* leafnode_copy(LeafNode* base, ic_Version version){
    LeafNode* leaf = malloc(sizeof(*leaf));
    leaf->version = version;
    for (unsigned int i = 0; i < LEAF_NODE_SIZE; i++) {
        leaf->values[i] = base->values[i];
    }
    return leaf;
}

IndexNode* indexnode_create(ic_Version version){
    IndexNode* node = malloc(sizeof(*node));
    node->version = version;
    for(unsigned int i = 0; i < INDEX_NODE_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

IndexNode* indexnode_copy(IndexNode* base, ic_Version version){
    IndexNode* node = malloc(sizeof(*node));
    node->version = version;
    memcpy(node->children, base->children, sizeof(*node->children) * INDEX_NODE_SIZE);
    return node;
}


void node_delete(IndexNode* node, ic_Version version, unsigned int depth) {
    if (node == NULL || node->version != version) {
        return;
    }
    if (depth != 0) {
        for(unsigned int i = 0; i < INDEX_NODE_SIZE; i++) {
            node_delete(node->children[i], version, depth - 1);
        }
    }
    free(node);
}

size_t node_size(IndexNode* node, ic_Version version, unsigned int depth) {
    if(node == NULL || node->version != version){
        return 0;
    }
    if (depth == 0) {
        return sizeof(LeafNode);
    }
    size_t size = sizeof(*node);
    for(unsigned int i = 0; i < INDEX_NODE_SIZE; i++) {
        size += node_size(node->children[i], version, depth - 1);
    }
    return size;
}

void node_dump(IndexNode* node, unsigned int depth, unsigned int indent){
    for(int i=0; i< indent; i++){
        printf("    ");
    }
    if(node == NULL){
        printf("NULL\n");
        return;
    }
    if(depth == 0){
        LeafNode* leaf = (LeafNode*)node;
        printf("<%p> ", leaf);
        for(int i = 0; i < LEAF_NODE_SIZE; i++){
            ic_Document* doc = leaf->values[i];
            if (doc == NULL){
                printf("-, ");
            }
            else{
                printf("%llu, ", doc->id);
            }
        }
        printf("\n");
    }
    else{
        printf("<%p> version=%llu\n", node, node->version);
        for(unsigned int i = 0; i < INDEX_NODE_SIZE; i++) {
            node_dump(node->children[i], depth - 1, indent + 1);
        }
    }
}

void node_walk(IndexNode* node, unsigned int depth, void (*callback)(ic_Document* value)) {
    if (node == NULL){
        return;
    }
    if (depth == 0){
        LeafNode* leaf = (LeafNode*)node;
        for(int i = 0; i < LEAF_NODE_SIZE; i++){
            ic_Document* value = leaf->values[i];
            if (value != NULL){
                callback(value);
            }
        }
    }
    else{
        for(unsigned int i = 0; i < INDEX_NODE_SIZE; i++) {
            node_walk(node->children[i], depth - 1, callback);
        }
    }
}


Tree* tree_create(ic_Version version, Tree* base) {
    Tree* tree = malloc(sizeof(*tree));
    tree->version = version;
    IndexNode* root;
    
    if (base != NULL) {
        root = indexnode_copy(base->root, version);
        tree->depth = base->depth;
    }
    else{
        root = indexnode_create(version);
        tree->depth = 1;
    }

    tree->root = root;
    return tree;
}

size_t tree_capacity(Tree* tree){
    return LEAF_NODE_SIZE * (1 << tree->depth);
}

unsigned int get_child_index(ic_Id x, int i) {
    return (x & (INDEX_NODE_MASK << (INDEX_NODE_BITS * i))) >> (INDEX_NODE_BITS * i);
}

void tree_insert(Tree* tree, ic_Id id, ic_Document* value) {
    size_t capacity = tree_capacity(tree);
    while(id >= capacity){
        IndexNode* new_root = indexnode_create(tree->version);
        new_root->children[0] = tree->root;
        tree->root = new_root;
        tree->depth += 1;
        capacity *= INDEX_NODE_SIZE;
    }
    
    ic_Id x = (id - 1) / LEAF_NODE_SIZE;
    IndexNode* node = tree->root;

    for(int i = tree->depth - 1; i >= 0; i--) {
        unsigned int child_index = get_child_index(x, i);
        IndexNode* child = node->children[child_index];
        if (child == NULL){
            if (i == 0) {
                child = (IndexNode*)leafnode_create(tree->version);
            }
            else {
                child = indexnode_create(tree->version);
            }
            node->children[child_index] = child;
        }
        if (child->version != tree->version) {
            if (i == 0){
                child = (IndexNode*)leafnode_copy((LeafNode*)child, tree->version);
            }
            else{
                child = indexnode_copy(child, tree->version);;
            }
            node->children[child_index] = child;
        }
        node = child;
    }
    //printf("insert id=%llu version=%llu (depth=%u, index=%llu)\n", id, tree->version, tree->depth, id % LEAF_NODE_SIZE);
    LeafNode* leaf = (LeafNode*) node;
    leaf->values[(id - 1) % LEAF_NODE_SIZE] = value;
}

ic_IndexError tree_get(Tree* tree, ic_Id id, ic_Document** value){
    if (id > tree_capacity(tree)){
        *value = NULL;
        return DOCUMENT_DOES_NOT_EXIST;
    }
    IndexNode* node = tree->root;
    ic_Id x = id / LEAF_NODE_SIZE;
    for(int i = tree->depth - 1; i >= 0; i--) {
        unsigned int child_index = get_child_index(x, i);
        IndexNode* child = node->children[child_index];
        if (child == NULL){
            return DOCUMENT_DOES_NOT_EXIST;
        }
        node = child;
    }
    LeafNode* leaf = (LeafNode*) node;
    *value = leaf->values[id % LEAF_NODE_SIZE];
    return OK;
}

void tree_delete(Tree* tree) {
    node_delete(tree->root, tree->version, tree->depth);
    free(tree);
}

size_t tree_size(Tree* tree) {
    return sizeof(*tree) + node_size(tree->root, tree->version, tree->depth);
}

void tree_dump(Tree* tree){
    node_dump(tree->root, tree->depth, 0);
}

void tree_walk(Tree* tree, void (*callback)(ic_Document* value)) {
    node_walk(tree->root, tree->depth, callback);
}

typedef struct{
    IndexNode* node;
    size_t index;
} TreeCursorStackFrame;

typedef struct{
    TreeCursorStackFrame* stack;
    TreeCursorStackFrame* stack_end;
    Tree* tree;
    size_t depth;
} TreeCursorMemo;

void _tree_cursor_next(void* memo, ic_Document** value) {
    TreeCursorMemo* m = (TreeCursorMemo*)memo;
    TreeCursorStackFrame* frame = m->stack;
    
    if (frame->index == LEAF_NODE_SIZE) {
        while (frame != m->stack_end) {
            // move up the stack until we find a node that has remaining child nodes
            frame++;
            if (frame->index < INDEX_NODE_SIZE - 1) {
                // pick the next branch
                frame->index++;
                while (frame != m->stack){
                    // move down the stack until we reach a leaf node
                    IndexNode* child = frame->node->children[frame->index];
                    frame--;
                    frame->node = child;
                    frame->index = 0;
                }
                break;
            }
            // all index nodes have been visited. cleanup
            *value = NULL;
            goto done;
        }
    }
    LeafNode* leaf = (LeafNode*)frame->node;
    *value = leaf->values[frame->index++];
    if (*value != NULL) {
        return;
    }
done:
    free(m->stack);
    free(m);
}

ic_Cursor* tree_cursor(Tree* tree) {
    TreeCursorMemo* memo = malloc(sizeof(*memo));
    memo->tree = tree;
    memo->stack = calloc(sizeof(TreeCursorStackFrame), tree->depth + 1);
    memo->stack_end = memo->stack + tree->depth;
    
    IndexNode* node = tree->root;
    for (TreeCursorStackFrame* frame = memo->stack_end; frame >= memo->stack; frame--) {
        frame->node = node;
        frame->index = 0;
        node = node->children[0];
    }
    
    return ic_cursor_create(&_tree_cursor_next, memo);
}


ic_DocumentVersionIndex* ic_dvidx_create() {
    ic_DocumentVersionIndex* idx = malloc(sizeof(*idx));
    idx->trees = calloc(MAX_VERSIONS, sizeof(*idx->trees));
    idx->trees[0] = tree_create(0, NULL);
    idx->last_version = 0;
    return idx;
}

ic_IndexError ic_dvidx_insert(ic_DocumentVersionIndex* idx, ic_Id id, ic_Version version, ic_Document* value) {
    assert(version > 0);
    if (version > idx->last_version + 1) {
        return VERSION_SKIPPED;
    }
    Tree* tree = idx->trees[version];
    if (tree == NULL) {
        tree = tree_create(version, idx->trees[version - 1]);
        idx->trees[version] = tree;
    }
    tree_insert(tree, id, value);
    idx->last_version = version;
    return 0;
}


ic_IndexError ic_dvidx_get(ic_DocumentVersionIndex* idx, ic_Id id, ic_Version version, ic_Document** value){
    //printf("getting document id=%llu version=%llu\n", id, version);
    if (version > idx->last_version) {
        return VERSION_DOES_NOT_EXIST;
    }
    Tree* tree = idx->trees[version];
    if (tree == NULL){
        return VERSION_DOES_NOT_EXIST;
    }
    return tree_get(tree, id, value);
}

size_t ic_dvidx_size(ic_DocumentVersionIndex* idx){
    size_t size = sizeof(*idx) + idx->last_version * sizeof(*idx->trees);
    for (int i = idx->last_version; i >= 0; i--) {
        size += tree_size(idx->trees[i]);
    }
    return size;
}

void ic_dvidx_delete(ic_DocumentVersionIndex* idx) {
    for (int i = idx->last_version; i >= 0; i--) {
        tree_delete(idx->trees[i]);
    }
    free(idx->trees);
    free(idx);
}

ic_Cursor* ic_dvidx_cursor(ic_DocumentVersionIndex* idx, ic_Version version) {
    Tree* tree = idx->trees[version];
    return tree_cursor(tree);
}

void ic_dvidx_walk(ic_DocumentVersionIndex* idx, ic_Version version, void (*callback)(ic_Document* value)) {
    Tree* tree = idx->trees[version];
    tree_walk(tree, callback);
}

