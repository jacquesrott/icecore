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
} Icecore;

Icecore* icecore_create(const char* path);
void icecore_next_version(Icecore* ic);
Document* icecore_insert(Icecore* ic, Id id, char* data);
ice_t icecore_get(Icecore* ic, Id id, Version version, Document** doc);

#endif
