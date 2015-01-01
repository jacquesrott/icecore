#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sodium.h>
#include "md5.h"

#include "filestore.h"
#include "utils.h"
#include "document.h"

const mode_t DEFAULT_MODE = 0755;


char* path_join(const char* root, const char* next) {
    char* path = malloc(sizeof(char) * (strlen(root) + strlen(next)));
    strcpy(path, root);
    strcat(path, next);

    return path;
}


FileStore* filestore_create(const char* path) {
    FileStore* filestore = malloc(sizeof(*filestore));
    filestore->root = malloc(sizeof(char) * strlen(path) + 1);
    strcpy(filestore->root, path);

    if(mkdir(path, DEFAULT_MODE) == -1 && errno != EEXIST) {
        printf("Folder creation failed. ERROR: %d\n", errno);
        return NULL;
    }

    return filestore;
}


void filestore_delete(FileStore* filestore) {
    free(filestore->root);
    free(filestore);
}


Hash filestore_write(const FileStore* filestore, const char* data) {
    int size = strlen(data);
    Hash hash;
    MD5_CTX ctx;

    MD5_Init(&ctx);
    MD5_Update(&ctx, data, size);
    MD5_Final(hash.bytes, &ctx);

    HexHash hex_hash = hash_to_hex(hash);
    char* path = path_join(filestore->root, hex_hash.chars);

    FILE* file = fopen(path, "w");

    if(file == NULL) {
        printf("Failed to open %s. ERROR: %d\n", path, errno);
        exit(EXIT_FAILURE);
    }

    fputs(data, file);

    fclose(file);
    free(path);

    return hash;
}


char* filestore_read(const FileStore* filestore, Hash hash) {
    HexHash hex_hash = hash_to_hex(hash);
    char* path = path_join(filestore->root, hex_hash.chars);
    FILE* file = fopen(path, "r");

    if(file == NULL) {
        printf("Failed to open %s. ERROR: %d\n", path, errno);
        return NULL;
    }
    long  file_size = get_file_size(file);
                    
    char* content = malloc(file_size);
    fread(content, file_size, 1, file);

    fclose(file);
    free(path);

    return content;
}
