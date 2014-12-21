#ifndef IC_VERSIONINDEX_H
#define IC_VERSIONINDEX_H

#include <stdint.h>
#include <string.h>

#include "document.h"
#include "cursor.h"
#include "error.h"
#include "mtree.h"


typedef struct {
    Version last_version;
    MTree* version_tree;
} DocumentVersionIndex;


DocumentVersionIndex* versionindex_create();
ice_t versionindex_insert(DocumentVersionIndex* idx, Id id, Version version, Document* value);
ice_t versionindex_get(DocumentVersionIndex* idx, Id id, Version version, Document** value);
size_t versionindex_size(DocumentVersionIndex* idx);
void versionindex_delete(DocumentVersionIndex* idx);
Cursor* versionindex_cursor(DocumentVersionIndex* idx, Version version);

#endif