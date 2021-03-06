#include <stdio.h>
#include <string.h>
#include "../filestore.h"


void test_filestore() {
    char path[] = "/tmp/test-filestore/";
    FileStore* store = filestore_create((const char*)path);
    sput_fail_if(store == NULL, "File storage had an error");
    filestore_delete(store);

    store = filestore_create((const char*)path);
    sput_fail_unless(store != NULL, "File storage created when folder exists.");

    int cmp = strcmp(store->root, path);
    sput_fail_unless(cmp == 0, "File storage gets /tmp/test-filestore/ assigned as root folder");

    char content[] = "test data";

    char* hash = filestore_write(store, content);
    char* stored_content = filestore_read(store, hash);
    sput_fail_if(stored_content == NULL, "File could be opened");

    printf("Hash: %s\n", hash);
    printf("Content: %s\n", stored_content);

    cmp = strcmp(content, stored_content);
    sput_fail_unless(cmp == 0, "File storage wrote the right content into proper file");

    filestore_delete(store);
    free(hash);
    free(stored_content);
}
