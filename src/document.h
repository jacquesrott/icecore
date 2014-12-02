#ifndef IC_DOCUMENT_H
#define IC_DOCUMENT_H

#include <stdint.h>

typedef struct{
    uint64_t id;
    uint64_t version;
} ic_Document;

#endif