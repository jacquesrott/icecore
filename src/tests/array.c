#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../array.h"


void test_array() {
    Array* array = array_create();

    sput_fail_if(array == NULL, "Array created");
    int len = array_length(array);
    sput_fail_unless(len == 0, "Array creation sets length to 0");

    char item[] = "test";
    array_append(array, (void*) &item);

    len = array_length(array);
    sput_fail_unless(len == 1, "Append increments array length");

    char* stored_item = array_get(array, 0);
    int cmp = strcmp(stored_item, item);
    sput_fail_unless(cmp == 0, "Array returns proper value");

    char new_item[] = "test-2";
    array_set(array, 0, (void*) &new_item);

    stored_item = array_get(array, 0);
    cmp = strcmp(stored_item, new_item);
    sput_fail_unless(cmp == 0, "Array returns proper value when set");

    int has = array_has(array, new_item);
    sput_fail_unless(has == 0, "Array contains item");

    has = array_has(array, item);
    sput_fail_unless(has == 1, "Array doesn't contains item");

    array_delete(array);
}


void test_big_array() {
    Array* array = array_create();

    sput_fail_if(array == NULL, "Array created");

    int i;
    char** strings = malloc(sizeof(char) * (7 + 3) * MAX_ARRAY_SIZE);
    for(i = 0; i < MAX_ARRAY_SIZE; i++) {
        char* string = malloc(sizeof(char) * (7 + 3));
        sprintf(string, "test-%d", i);
        strings[i] = string;
        array_append(array, strings[i]);
    }

    int len = array_length(array);
    sput_fail_unless(len == MAX_ARRAY_SIZE, "Array creation sets length to max size");

    char item[] = "test-max-size";
    array_append(array, item);

    len = array_length(array);
    sput_fail_unless(len == (MAX_ARRAY_SIZE + 1), "Array can go over max size");

    char* stored_item = array_pop(array);
    int cmp = strcmp(stored_item, item);
    sput_fail_unless(cmp == 0, "Array returns proper value when pop");

    array_append(array, item);
    stored_item = array_pop_index(array, MAX_ARRAY_SIZE);
    cmp = strcmp(stored_item, item);
    sput_fail_unless(cmp == 0, "Array returns proper value when pop specific index");

    len = array_length(array);
    sput_fail_unless(len == MAX_ARRAY_SIZE, "Array has proper length after pop index");

    array_append(array, item);
    int removed = array_remove(array, item);
    sput_fail_unless(removed == 0, "Array removes existing element");

    int has = array_has(array, item);
    sput_fail_unless(has == 1, "Array doesn't contain removed element");

    removed = array_remove(array, item);
    sput_fail_unless(removed == 1, "Array doesn't remove non existing element");

    len = array_length(array);
    sput_fail_unless(len == MAX_ARRAY_SIZE, "Array has proper length after remove");

    array_append(array, item);
    stored_item = array_get(array, -1);
    cmp = strcmp(stored_item, item);
    sput_fail_unless(cmp == 0, "Array returns proper value when using negative index");
    array_remove(array, item);

    for(i = 0; i < MAX_ARRAY_SIZE; i++) {
        free(strings[i]);
    }
    free(strings);
}
