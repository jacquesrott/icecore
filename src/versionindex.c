#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "versionindex.h"
#include "document.h"
#include "cursor.h"
#include "error.h"
#include "mtree.h"


DocumentVersionIndex* versionindex_create() {
    DocumentVersionIndex* idx = malloc(sizeof(*idx));
    idx->version_tree = mtree_create(4, 4, false, NULL);
    idx->last_version = 0;
    return idx;
}


void versionindex_next_version(DocumentVersionIndex* idx) {
    MTree* previous = NULL;
    if (idx->last_version > 0) {
        mtree_get(idx->version_tree, idx->last_version, (void**)&previous);
        assert(previous != NULL);
    }
    MTree* tree = mtree_create(4, 4, true, previous);
    mtree_insert(idx->version_tree, ++idx->last_version, tree);
}


ice_t versionindex_insert(DocumentVersionIndex* idx, Id id, Document* value) {
    MTree* versioned_tree;
    mtree_get(idx->version_tree, idx->last_version, (void**)&versioned_tree);
    mtree_insert(versioned_tree, id, value);
    return ICE_OK;
}


ice_t versionindex_get(DocumentVersionIndex* idx, Id id, Version version, Document** value){
    assert(idx->last_version >= version);
    printf("getting document id=%llu version=%llu\n", id, version);
    if (version > idx->last_version) {
        return ICE_VERSION_DOES_NOT_EXIST;
    }
    MTree* versioned_tree;
    mtree_get(idx->version_tree, version, (void**)&versioned_tree);
    if (versioned_tree == NULL){
        return ICE_VERSION_DOES_NOT_EXIST;
    }
    return mtree_get(versioned_tree, id, (void**)value);
}

size_t versionindex_size(DocumentVersionIndex* idx){
    return 0;
}

void versionindex_delete(DocumentVersionIndex* idx) {
    mtree_walk(idx->version_tree, (TreeWalkCallback)&mtree_delete);
    mtree_delete(idx->version_tree);
    free(idx);
}

Cursor* versionindex_cursor(DocumentVersionIndex* idx, Version version) {
    MTree* versioned_tree;
    mtree_get(idx->version_tree, version, (void**)&versioned_tree);
    return mtree_cursor(versioned_tree);
}

void versionindex_walk(DocumentVersionIndex* idx, Version version, void (*callback)(Document* value)) {
    MTree* versioned_tree;
    mtree_get(idx->version_tree, version, (void**)&versioned_tree);
    mtree_walk(versioned_tree, (TreeWalkCallback)callback);
}

