#ifndef IC_ARRAY_H
#define IC_ARRAY_H

#define MAX_ARRAY_SIZE 100

struct Array;
typedef struct Array Array;

Array* array_create();
void array_delete(Array* array);

void array_append(Array* array, void* item);
void* array_pop(Array* array);
void* array_pop_index(Array* array, int i);
int array_length(const Array* array);
void* array_get(const Array* array, int i);
void array_set(Array* array, int i, void* value);

int array_has(const Array* array, const void* item);
int array_index(const Array* array, const void* item);
int array_remove(Array* array, const void* item);

#endif
