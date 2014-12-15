#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sodium.h>

#include "filestore.h"

const mode_t DEFAULT_MODE = 0755;


long get_file_size(FILE *file) {
    long  file_size;
    
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return file_size;
}

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

    if (sodium_init() == -1) {
        printf("Sodium couldn't initialise properly.\n");
        exit(EXIT_FAILURE);
    }

    return filestore;
}


void filestore_delete(FileStore* filestore) {
    free(filestore->root);
    free(filestore);
}


char* filestore_write(const FileStore* filestore, const char* data) {
    unsigned char hash[crypto_generichash_BYTES];
    unsigned char key[crypto_generichash_KEYBYTES];
    randombytes_buf(key, sizeof(key));

    // Blake2b algorithm
    crypto_generichash(
            hash, sizeof(hash),
            (const unsigned char*)data, sizeof(data),
            key, sizeof(key));

    size_t hex_hash_max_len = crypto_generichash_BYTES * 2 + 1;
    char* hex_hash = malloc(hex_hash_max_len);
    hex_hash = sodium_bin2hex(hex_hash, hex_hash_max_len, hash, sizeof(hash));

    char* path = path_join(filestore->root, hex_hash);

    FILE* file = fopen(path, "w");

    if(file == NULL) {
        printf("Failed to open %s. ERROR: %d\n", path, errno);
        exit(EXIT_FAILURE);
    }

    fputs(data, file);

    fclose(file);
    free(path);

    return hex_hash;
}


char* filestore_read(const FileStore* filestore, const char* hash) {
    char* path = path_join(filestore->root, hash);
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
