#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cursor.h"
#include "document.h"
#include "versionindex.h"
#include "error.h"
#include "filestore.h"
#include "icecore.h"


Icecore* icecore_create(const char* path) {
    Icecore* ic = malloc(sizeof(*ic));
    ic->file_store = filestore_create(path);
    ic->vindex = versionindex_create();
    return ic;
}

void icecore_next_version(Icecore* ic) {
    versionindex_next_version(ic->vindex);
}

Document* icecore_insert(Icecore* ic, Id id, char* data) {
    char* hash = filestore_write(ic->file_store, data);
    Document* doc = malloc(sizeof(*doc));
    doc->id = id;
    doc->version = ic->vindex->last_version;
    doc->hash = hash;
    versionindex_insert(ic->vindex, id, doc);
    return doc;
}

ice_t icecore_get(Icecore* ic, Id id, Version version, Document** doc) {
    //printf("icecore_get()\n");
    return versionindex_get(ic->vindex, id, version, doc);
}

