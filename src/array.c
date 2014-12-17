#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "array.h"


struct Array {
    int length;
    int max;
    void** items;
};


Array* array_create() {
    Array* array = malloc(sizeof(*array));
    array->max = MAX_ARRAY_SIZE;
    array->length = 0;
    array->items = calloc(MAX_ARRAY_SIZE, sizeof(void*));
    return array;
}


static void array_resize(Array* array, int new_max) {
    int new_size = sizeof(void*) * new_max;
    void* result = realloc(array->items, new_size);
    if(result == NULL) {
        printf("Array reallocation issue for size %d.\n", new_size);
        exit(EXIT_FAILURE);
    }
    array->max = new_max;
    free(result);
}


static int array_validate_index(const Array* array, int i) {
    if(i < 0) {
        i = array->length + i;
    }
    if(i >= array->length || i < 0) {
        printf("Index %d out of range of length %d\n", i, array->length);
        exit(EXIT_FAILURE);
    }
    return i;
}


static void array_increase_max(Array* array) {
    if(array->length - 1 >= array->max) {
        int new_max = array->max + MAX_ARRAY_SIZE;
        array_resize(array, new_max);
    }
}


static void array_decrease_max(Array* array) {
    if(array->length - 1 < array->max - MAX_ARRAY_SIZE) {
        int new_size = array->max - MAX_ARRAY_SIZE;
        array_resize(array, new_size);
    }
}


void array_delete(Array* array) {
    free(array->items);
    free(array);
}


void array_append(Array* array, void* item) {
    array_increase_max(array);
    array->items[array->length] = item;
    array->length++;
}


void* array_pop(Array* array) {
    if(array->length <= 0) {
        return NULL;
    }

    int last = array->length - 1;
    void* item = array->items[last];
    array->items[last] = NULL;
    array->length--;

    array_decrease_max(array);

    return item;
}


void* array_pop_index(Array* array, int i) {
    i = array_validate_index(array, i);
    if(i == array->length - 1) {
        return array_pop(array);
    }
    void* item = array->items[i];

    int j;
    for(j = i; j < array->length - 1 ; j++) {
        array->items[j] = array->items[j + 1];
    }

    array->items[j] = NULL;
    array->length--;

    array_decrease_max(array);

    return item;
}


int array_length(const Array* array) {
    return array->length;
}


void* array_get(const Array* array, int i) {
    i = array_validate_index(array, i);
    return array->items[i];
}


void array_set(Array* array, int i, void* value) {
    i = array_validate_index(array, i);
    array->items[i] = value;
}


int array_index(const Array* array, const void* item) {
    int i;
    for(i = 0; i < array->length; i++) {
        if((const void*) &array->items[i] == &item || array->items[i] == item)
            return i;
    }
    return -1;
}


int array_has(const Array* array, const void* item) {
    int index = array_index(array, item);
    if(index == -1) {
        return 1;
    }
    return 0;
}


int array_remove(Array* array, const void* item) {
    int index = array_index(array, item);
    if(index == -1) {
        return 1;
    }
    array_pop_index(array, index);
    return 0;
}
