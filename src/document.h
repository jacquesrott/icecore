#ifndef IC_DOCUMENT_H
#define IC_DOCUMENT_H

#include <stdint.h>


#define DOCUMENT_HEX_HASH_SIZE 33

typedef uint64_t Version;
typedef uint64_t Id;

typedef union {
    struct {
        uint64_t a;
        uint64_t b;
    };
    unsigned char bytes[16];
} Hash;

typedef struct {
    char chars[DOCUMENT_HEX_HASH_SIZE];
} HexHash;

typedef struct{
    Id id;
    Version version;
    Hash hash;
} Document;


void get_hex_hash(Hash hash, char* hexhash);

HexHash hash_to_hex(Hash hash);

#endif