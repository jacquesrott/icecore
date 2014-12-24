#ifndef IC_DOCUMENT_H
#define IC_DOCUMENT_H

#include <stdint.h>

typedef uint64_t Version;
typedef uint64_t Id;

typedef union {
    struct {
        uint64_t a;
        uint64_t b;
    };
    unsigned char bytes[16];
} Hash;

typedef struct{
    Id id;
    Version version;
    Hash hash;
} Document;


#define DOCUMENT_HEX_HASH_SIZE 33
void get_hex_hash(Hash hash, char* hexhash);

#endif