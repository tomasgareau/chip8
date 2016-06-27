#ifndef __LIST_H
#define __LIST_H

#define LIST_INITIAL_CAPACITY 100

typedef struct {
    int size;
    int capacity;
    int data_size;
    void **data;
} list_t;

void init_list(list_t *, int data_size);
void list_append(list_t *, void *val);
void *list_get(list_t *, int index);
void destroy_list(list_t *);
int list_size(list_t *);

#endif
