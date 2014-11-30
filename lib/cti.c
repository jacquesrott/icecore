#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "cti.h"

const unsigned int LEAF_NODE_SIZE = 16;
const unsigned int MAX_VERSIONS = 1000;


typedef struct{
    Version version;
    Value values[LEAF_NODE_SIZE];
} LeafNode;



LeafNode* leafnode_create(Version version){
    LeafNode* leaf = malloc(sizeof(*leaf));
    leaf->version = version;
    for (unsigned int i = 0; i < LEAF_NODE_SIZE; i++) {
        leaf->values[i] = NONE;
    }
    return leaf;
}

LeafNode* leafnode_copy(LeafNode* base, Version version){
    LeafNode* leaf = malloc(sizeof(*leaf));
    leaf->version = version;
    for (unsigned int i = 0; i < LEAF_NODE_SIZE; i++) {
        leaf->values[i] = base->values[i];
    }
    return leaf;
}

IndexNode* indexnode_create(Version version){
    IndexNode* node = malloc(sizeof(*node));
    node->version = version;
    for(unsigned int i = 0; i < INDEX_NODE_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

IndexNode* indexnode_copy(IndexNode* base, Version version){
    IndexNode* node = malloc(sizeof(*node));
    node->version = version;
    memcpy(node->children, base->children, sizeof(*node->children) * INDEX_NODE_SIZE);
    return node;
}


void node_delete(IndexNode* node, Version version, unsigned int depth) {
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

size_t node_size(IndexNode* node, Version version, unsigned int depth) {
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
        for(int i=0;i<LEAF_NODE_SIZE;i++){
            printf("%llu, ", leaf->values[i]);
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

void node_walk(IndexNode* node, unsigned int depth, void (*callback)(Value value)) {
    if (node == NULL){
        return;
    }
    if (depth == 0){
        LeafNode* leaf = (LeafNode*)node;
        for(int i = 0; i < LEAF_NODE_SIZE; i++){
            Value value = leaf->values[i];
            if (value != NONE){
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


Tree* tree_create(Version version, Tree* base) {
    Tree* tree = malloc(sizeof(*tree));
    tree->version = version;
    IndexNode* root = indexnode_create(version);
    
    if (base != NULL) {
        for(unsigned int i = 0; i < INDEX_NODE_SIZE; i++) {
            root->children[i] = base->root->children[i];
        }
        tree->depth = base->depth;
    }
    else{
        tree->depth = 1;
    }

    tree->root = root;
    return tree;
}

size_t tree_capacity(Tree* tree){
    return LEAF_NODE_SIZE * (1 << tree->depth);
}

unsigned int get_child_index(Id x, int i) {
    return (x & INDEX_NODE_MASK << (INDEX_NODE_SIZE * i)) >> (INDEX_NODE_SIZE * i);
}

void tree_insert(Tree* tree, Id id, Value value) {
    size_t capacity = tree_capacity(tree);
    while(id >= capacity){
        IndexNode* new_root = indexnode_create(tree->version);
        new_root->children[0] = tree->root;
        tree->root = new_root;
        tree->depth += 1;
        capacity *= 2;
    }
    
    Id x = id / LEAF_NODE_SIZE;
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
    printf("insert (depth=%u, index=%llu)\n", tree->depth, id % LEAF_NODE_SIZE);
    LeafNode* leaf = (LeafNode*) node;
    leaf->values[id % LEAF_NODE_SIZE] = value;
}

IndexError tree_get(Tree* tree, Id id, Value* value){
    if (id > tree_capacity(tree)){
        return NONE;
    }
    IndexNode* node = tree->root;
    Id x = id / LEAF_NODE_SIZE;
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

void tree_walk(Tree* tree, void (*callback)(Value value)) {
    node_walk(tree->root, tree->depth, callback);
}


Index* index_create() {
    Index* idx = malloc(sizeof(*idx));
    idx->trees = calloc(MAX_VERSIONS, sizeof(*idx->trees));
    idx->trees[0] = tree_create(0, NULL);
    idx->last_version = 0;
    return idx;
}

IndexError index_insert(Index* idx, Id id, Version version, Value value) {
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


IndexError index_get(Index* idx, Id id, Version version, Value* value){
    if (version > idx->last_version) {
        return VERSION_DOES_NOT_EXIST;
    }
    Tree* tree = idx->trees[version];
    if (tree == NULL){
        return VERSION_DOES_NOT_EXIST;
    }
    return tree_get(tree, id, value);
}

size_t index_size(Index* idx){
    size_t size = sizeof(*idx) + idx->last_version * sizeof(*idx->trees);
    for (int i = idx->last_version; i >= 0; i--) {
        size += tree_size(idx->trees[i]);
    }
    return size;
}

void index_delete(Index* idx) {
    for (int i = idx->last_version; i >= 0; i--) {
        tree_delete(idx->trees[i]);
    }
    free(idx->trees);
    free(idx);
}


void index_walk(Index* idx, Version version, void (*callback)(Value value)) {
    Tree* tree = idx->trees[version];
    tree_walk(tree, callback);
}


void print_value(Value value){
    printf("%llu\n", value);
}


int main(int argc, char** argv) {
    Index* idx = index_create();
    for(int i=1;i<50;i++){
        index_insert(idx, 2*i, i, i);
        index_insert(idx, 2*i+1, i, i);
        index_insert(idx, i, i, 2*i);
    }

    for(int i=1;i<50;i++){
        Value val;
        int error = index_get(idx, i, 20, &val);
        printf("%i: %llu (error=%i)\n", i, val, error);
    }
    index_walk(idx, 40, &print_value);
    tree_dump(idx->trees[40]);
    printf("index size = %zu bytes\n", index_size(idx));
    index_delete(idx);
}
