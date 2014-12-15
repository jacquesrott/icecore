#ifndef IC_FILESTORE_H
#define IC_FILESTORE_H

#include <stdlib.h>

typedef struct {
    char* root;
} FileStore;


FileStore* filestore_create(const char* path);
void filestore_delete(FileStore* filestore);
char* filestore_read(const FileStore* filestore, const char* hash);
char* filestore_write(const FileStore* filestore, const char* data);

#endif
