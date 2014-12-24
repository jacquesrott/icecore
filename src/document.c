#include <stdio.h>
#include <string.h>

#include "document.h"


void get_hex_hash(Hash hash, char* str) {
    for (int i = 0; i < 16; i++) {
        sprintf(str + 2*i, "%02x", hash.bytes[i]);
    }
    str[32] = '\0';
}
