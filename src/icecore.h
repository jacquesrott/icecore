#ifndef IC_ICECORE_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "document.h"
#include "versionindex.h"
#include "error.h"
#include "filestore.h"


typedef struct Icecore{
    FileStore* file_store;
    DocumentVersionIndex* vindex;
    Id next_id;
} Icecore;

Icecore* icecore_create(const char* path);
void icecore_next_version(Icecore* ic);
Document* icecore_insert(Icecore* ic, char* data);
Document* icecore_insert_from_file(Icecore* ic, FILE* file);
Document* icecore_update(Icecore* ic, Id id, char* data);
//Document* icecore_update_from_file(Icecore* ic, Id id, FILE* file);
ice_t icecore_get(Icecore* ic, Id id, Version version, Document** doc);
//ice_t icecore_get_latest(Icecore* ic, Id id, Document** doc);

#endif
