#ifndef __CHIP8_H
#define __CHIP8_H

#include "stack.h"

#define MEMORY_SZ 4096
#define PROG_MEM_START 512
#define NUM_REGISTERS 16
#define NUM_KEYS 16
#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define FONT_WIDTH 4
#define FONT_HEIGHT 5
#define SCALE 10

#define FILE_OPEN_ERROR 1
#define FILE_SIZE_ERROR 2
#define BUFFER_SIZE_ERROR 3

typedef struct chip8_t
{
    // allocate 4K of memory
    unsigned char memory[MEMORY_SZ];

    // allocate memory for 16 8-bit registers named V0 to VF
    unsigned char V[NUM_REGISTERS];

    // allocate memory for the address register I
    unsigned short I;

    // declare the stack
    stack_t stack;

    // allocate memory for the program counter
    unsigned short pc;

    // allocate memory for the delay & sound timer
    unsigned char delay_timer;
    unsigned char sound_timer;

    // hex keyboard for input
    unsigned char key[NUM_KEYS];

    // allocate space for the graphics
    // each pixel can be either black or white (0 or 1)
    unsigned char gfx[GFX_WIDTH * GFX_HEIGHT];

    // global variable to hold the current opcode
    // opcodes are two bytes
    unsigned short opcode;
} chip8_t;

void (*opcode_table[16])(chip8_t *chip8);
void (*chip8_arithmetic[16])(chip8_t *chip8);
void opcode_0(chip8_t *chip8);
void opcode_1(chip8_t *chip8);
void opcode_2(chip8_t *chip8);
void opcode_3(chip8_t *chip8);
void opcode_4(chip8_t *chip8);
void opcode_5(chip8_t *chip8);
void opcode_6(chip8_t *chip8);
void opcode_7(chip8_t *chip8);
void opcode_8(chip8_t *chip8);
void opcode_9(chip8_t *chip8);
void opcode_A(chip8_t *chip8);
void opcode_B(chip8_t *chip8);
void opcode_C(chip8_t *chip8);
void opcode_D(chip8_t *chip8);
void opcode_E(chip8_t *chip8);
void opcode_F(chip8_t *chip8);
void illegal_func(chip8_t *chip8);
void arithmetic_0(chip8_t *chip8);
void arithmetic_1(chip8_t *chip8);
void arithmetic_2(chip8_t *chip8);
void arithmetic_3(chip8_t *chip8);
void arithmetic_4(chip8_t *chip8);
void arithmetic_5(chip8_t *chip8);
void arithmetic_6(chip8_t *chip8);
void arithmetic_7(chip8_t *chip8);
void arithmetic_E(chip8_t *chip8);
int init_chip8(chip8_t *chip8);
void emulate_cycle(chip8_t *chip8);
int load_game(chip8_t *chip8, const char *filename);

#endif
