#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

void init_list(list_t *list, int data_size) {
    list->size = 0;
    list->capacity = LIST_INITIAL_CAPACITY;
    list->data_size = data_size;
    list->data = malloc(sizeof(void *) * list->capacity);
}

void list_append(list_t *list, void *val) {
    if (list->size >= list->capacity) {
        // double list capacity
        list->capacity *= 2;
        list->data = realloc(list->data, list->data_size * list->capacity);
    }
    list->data[list->size] = malloc(list->data_size);
    memcpy(list->data[list->size++], val, list->data_size);
}

void *list_get(list_t *list, int index) {
    if (index >= list->size || index < 0) {
        fprintf(stderr, "Index %d is out of bounds for list of size %d\n",
                index, list->size);
        return NULL;
    }
    return list->data[index];
}

void destroy_list(list_t *list) {
    int i;
    for (i = 0; i < list->size; i++) {
        free(list->data[i]);
    }
    free(list->data);
}

int list_size(list_t *list) {
    return list->size;
}
