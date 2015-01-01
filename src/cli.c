#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <jansson.h>

#include "cursor.h"
#include "document.h"
#include "icecore.h"
#include "error.h"
#include "md5.h"


Icecore* cli_init() {
    Icecore* ic = icecore_create("_documents/");
    return ic;
}


void _show_doc(Document* doc) {
    char hexhash[DOCUMENT_HEX_HASH_SIZE];
    get_hex_hash(doc->hash, hexhash);
    printf("id: %llu\nversion: %llu\nhash: %s\n", doc->id, doc->version, hexhash);
}


int cli_insert(int nargs, char** args) {
    if (nargs == 0) {
        printf("At least one file name is required for `icecore insert\n");
        return 1;
    }
    Icecore* ic = cli_init();

    icecore_next_version(ic);
    FILE* file = fopen(args[0], "r");
    if (file == NULL) {
        printf("cannot open file %s %i\n", args[0], errno);
        return 1;
    }
    Document* doc = icecore_insert_from_file(ic, file);
    fclose(file);
    _show_doc(doc);
    return 0; 
}

int cli_update(int nargs, char** args) {
    if (nargs != 2) {
        printf("Usage: icecore update <id> <file>");
        return 1;
    }
    Icecore* ic = cli_init();
    icecore_next_version(ic);
    Id id = atoll(args[0]);
    if (id == 0) {
        printf("invalid document id: %s\n", args[0]);
        return 1;
    }
    FILE* file = fopen(args[1], "r");
    if (file == NULL) {
        printf("cannot open file %s %i\n", args[1], errno);
        return 1;
    }
    Document* doc = icecore_update_from_file(ic, id, file);
    fclose(file);
    _show_doc(doc);
    return 0; 
}

int cli_get(int nargs, char** args) {
    printf("get\n");
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
    
    Icecore* ic = cli_init();
    icecore_next_version(ic);
    icecore_insert(ic, "foo");
    icecore_insert(ic, "bar");
    icecore_insert(ic, "baz");
    
    icecore_next_version(ic);
    icecore_update(ic, 1, "foo2");
    
    icecore_next_version(ic);
    icecore_update(ic, 2, "bar2");
    
    Document* doc;
    icecore_get(ic, 1, 1, &doc);
    printf("id=%llu version=%llu\n", doc->id, doc->version);
    icecore_get(ic, 2, 1, &doc);
    printf("id=%llu version=%llu\n", doc->id, doc->version);
    
    icecore_get(ic, 1, 2, &doc);
    printf("id=%llu version=%llu\n", doc->id, doc->version);
    icecore_get(ic, 2, 2, &doc);
    printf("id=%llu version=%llu\n", doc->id, doc->version);

    icecore_get(ic, 1, 3, &doc);
    printf("id=%llu version=%llu\n", doc->id, doc->version);
    icecore_get(ic, 2, 3, &doc);
    printf("id=%llu version=%llu\n", doc->id, doc->version);

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