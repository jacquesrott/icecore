#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jansson.h>

#include "cursor.h"
#include "document.h"
#include "icecore.h"
#include "error.h"
#include "md5.h"


int cli_insert(int nargs, char** args) {
    printf("insert\n");
    return 0; 
}

int cli_get(int nargs, char** args) {
    printf("get\n");
    return 0; 
}

int cli_update(int nargs, char** args) {
    printf("update\n");
    return 0; 
}

int cli_help(int nargs, char** args) {
    printf("help\n");
    return 0; 
}

int cli_demo(int argc, char** argv) {
    json_error_t json_error;
    json_t* data = json_loads("{\"foo\": 42}", 0, &json_error);
    int val = json_integer_value(json_object_get(data, "foo"));
    printf("JSON VALUE = %i\n", val);
    
    Icecore* ic = icecore_create("_documents/");
    icecore_next_version(ic);
    icecore_insert(ic, 1, "foo");
    icecore_insert(ic, 2, "bar");
    icecore_insert(ic, 3, "baz");
    
    icecore_next_version(ic);
    icecore_insert(ic, 1, "foo2");
    
    icecore_next_version(ic);
    icecore_insert(ic, 2, "bar2");
    
    Document* doc;
    icecore_get(ic, 1, 1, &doc);
    printf("id=%llu version=%llu hash=%s\n", doc->id, doc->version, doc->hash);
    icecore_get(ic, 2, 1, &doc);
    printf("id=%llu version=%llu hash=%s\n", doc->id, doc->version, doc->hash);
    
    icecore_get(ic, 1, 2, &doc);
    printf("id=%llu version=%llu hash=%s\n", doc->id, doc->version, doc->hash);
    icecore_get(ic, 2, 2, &doc);
    printf("id=%llu version=%llu hash=%s\n", doc->id, doc->version, doc->hash);

    icecore_get(ic, 1, 3, &doc);
    printf("id=%llu version=%llu hash=%s\n", doc->id, doc->version, doc->hash);
    icecore_get(ic, 2, 3, &doc);
    printf("id=%llu version=%llu hash=%s\n", doc->id, doc->version, doc->hash);

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, "foo", 3);

    Hash h;
    MD5_Final((unsigned char*)&h, &ctx);
    printf("%016llX %016llX\n", h.a, h.b);
    for(int i=0;i<16;i++){
        printf("%x", h.bytes[i]);
    }
    printf("\n");
    return 0;
}