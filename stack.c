#include <stdio.h>
#include "stack.h"

void init_stack(stack_t *stk) {
    stk->sp = 0;
}

void stack_push(stack_t *stk, unsigned short val) {
    if (stk->sp < STACK_MAX) {
        stk->data[stk->sp++] = val;
    }
    else {
        fputs("Memory error: stack full.\n", stderr);
    }
}

unsigned short stack_pop(stack_t *stk) {
    if (stk->sp > 0) {
        return stk->data[--stk->sp];
    }
    else {
        fputs("Memory error: stack empty\n", stderr);
        // resets program counter to start of program
        return 0x200;
    }
}
