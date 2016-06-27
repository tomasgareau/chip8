#include <stdio.h>
#include "chip8.h"
#include "stack.h"

#define SUCCESS 0
#define FAILURE 1

int (*test_table[16])(chip8_t *chip8);

int (*chip8_test_arithmetic[16])(chip8_t *chip8);

int test_arithmetic_0 (chip8_t *);

int test_arithmetic_1 (chip8_t *);

int test_arithmetic_2 (chip8_t *);

int test_arithmetic_3 (chip8_t *);

int test_arithmetic_4 (chip8_t *);

int test_arithmetic_5 (chip8_t *);

int test_arithmetic_6 (chip8_t *);

int test_arithmetic_7 (chip8_t *);

int test_arithmetic_E (chip8_t *);

int illegal_func_int (chip8_t *);

int test_0(chip8_t *);

int test_1(chip8_t *);

int test_2(chip8_t *);

int test_3(chip8_t *);

int test_4(chip8_t *);

int test_5(chip8_t *);

int test_6(chip8_t *);

int test_7(chip8_t *);

int test_8(chip8_t *);

int test_9(chip8_t *);

int test_A(chip8_t *);

int test_B(chip8_t *);

int test_C(chip8_t *);

int test_D(chip8_t *);

int test_E(chip8_t *);

int test_F(chip8_t *);

int run_test(unsigned short);

