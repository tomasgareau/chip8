#ifndef _STACK_H
#define _STACK_H

#define STACK_MAX 16

typedef struct stack_t {
    unsigned short data[STACK_MAX];
    unsigned short sp;
} stack_t;

void init_stack(stack_t *);
void stack_push(stack_t *, unsigned short);
unsigned short stack_pop(stack_t *);

#endif
