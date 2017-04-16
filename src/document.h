#ifndef IC_DOCUMENT_H
#define IC_DOCUMENT_H

#include <stdint.h>

typedef uint64_t Version;
typedef uint64_t Id;

typedef struct{
    Id id;
    Version version;
    char* hash;
} Document;


typedef struct {
    uint64_t version;
} ic_Transaction;


typedef struct {
    uint64_t document_id;
    ic_Document** versions;
} ic_VersionIndex;


typedef struct {
    ic_Document** documents;
    ic_Document* last_document;
} ic_DocumentIndex;

#endif
