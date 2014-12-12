#ifndef IC_DOCUMENT_H
#define IC_DOCUMENT_H

#include <stdint.h>

typedef uint64_t Version;
typedef uint64_t Id;

typedef struct{
    Id id;
    Version version;
} Document;

#endif