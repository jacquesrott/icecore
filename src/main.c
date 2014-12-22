#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cursor.h"
#include "document.h"
#include "icecore.h"
#include "error.h"


int main(int argc, char** argv) {
    printf("welcome to icecore\n");
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

}

