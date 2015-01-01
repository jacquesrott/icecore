#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cursor.h"
#include "document.h"
#include "versionindex.h"
#include "error.h"
#include "filestore.h"
#include "utils.h"
#include "icecore.h"


Icecore* icecore_create(const char* path) {
    Icecore* ic = malloc(sizeof(*ic));
    ic->file_store = filestore_create(path);
    ic->next_id = 1;
    ic->vindex = versionindex_create();
    return ic;
}

void icecore_next_version(Icecore* ic) {
    versionindex_next_version(ic->vindex);
}

Document* icecore_update(Icecore* ic, Id id, char* data) {
    Hash hash = filestore_write(ic->file_store, data);
    Document* doc = malloc(sizeof(*doc));
    doc->id = id;
    doc->version = ic->vindex->last_version;
    doc->hash = hash;
    versionindex_insert(ic->vindex, id, doc);
    return doc;
}

Document* icecore_insert(Icecore* ic, char* data) {
    return icecore_update(ic, ic->next_id++, data);
}

Document* icecore_insert_from_file(Icecore* ic, FILE* file) {
    long size = get_file_size(file);
    char data[size + 1];
    data[size] = '\0';
    fread(data, size, 1, file);
    return icecore_insert(ic, data);
}

Document* icecore_update_from_file(Icecore* ic, Id id, FILE* file) {
    long size = get_file_size(file);
    char data[size + 1];
    data[size] = '\0';
    fread(data, size, 1, file);
    return icecore_update(ic, id, data);
}

ice_t icecore_get(Icecore* ic, Id id, Version version, Document** doc) {
    //printf("icecore_get()\n");
    return versionindex_get(ic->vindex, id, version, doc);
}

