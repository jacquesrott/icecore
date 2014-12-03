#ifndef IC_DOCUMENT_H
#define IC_DOCUMENT_H

#include <stdint.h>

typedef uint64_t ic_Version;
typedef uint64_t ic_Id;

typedef struct{
    ic_Id id;
    ic_Version version;
} ic_Document;

#endif