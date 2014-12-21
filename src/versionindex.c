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

ice_t versionindex_insert(DocumentVersionIndex* idx, Id id, Version version, Document* value) {
    //printf("versionindex_insert id=%llu, version=%llu, last_version=%llu\n", id, version, idx->last_version);
    assert(version != 0);
    assert(version >= idx->last_version);
    assert(version <= idx->last_version + 1);
    
    if (version > idx->last_version + 1) {
        return ICE_VERSION_SKIPPED;
    }
    
    MTree* versioned_tree;
    if (version == idx->last_version) {
        mtree_get(idx->version_tree, version, (void**)&versioned_tree);
        assert(versioned_tree != NULL);
    }
    else {
        MTree* previous = NULL;
        if (version > 1) {
            mtree_get(idx->version_tree, version - 1, (void**)&previous);
            assert(previous != NULL);
        }
        versioned_tree = mtree_create(4, 4, true, previous);
        mtree_insert(idx->version_tree, version, versioned_tree);
    }
    mtree_insert(versioned_tree, id, value);
    idx->last_version = version;
    return ICE_OK;
}


ice_t versionindex_get(DocumentVersionIndex* idx, Id id, Version version, Document** value){
    //printf("getting document id=%llu version=%llu\n", id, version);
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

