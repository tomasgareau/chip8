#include <stdio.h>
#include <string.h>
#include "chip8.h"
#include "stack.h"
#include "tests.h"

#define SUCCESS 0
#define FAILURE 1

int (*test_table[16])(chip8_t *chip8) =
{
    test_0, test_1, test_2, test_3, test_4, test_5, test_6,
    test_7, test_8, test_9, test_A, test_B, test_C, test_D,
    test_E, test_F
};

int (*chip8_test_arithmetic[16])(chip8_t *chip8) =
{
    test_arithmetic_0, test_arithmetic_1, test_arithmetic_2, test_arithmetic_3, test_arithmetic_4,
    test_arithmetic_5, test_arithmetic_6, test_arithmetic_7, illegal_func_int, illegal_func_int,
    illegal_func_int, illegal_func_int, illegal_func_int, illegal_func_int, test_arithmetic_E,
    illegal_func_int
};

int test_arithmetic_0(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY0
      Set VX = VY
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 0;
    chip8->V[y] = 10;
    opcode_8(chip8);
    if (chip8->V[x] == 10) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_arithmetic_1(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY1
      Set VX = VX (OR) VY
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 0x5F;
    chip8->V[y] = 0xAF;
    arithmetic_1(chip8);
    if (chip8->V[x] == 0x5F | 0xAF) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_arithmetic_2(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY2
      Set VX = VX (AND) VY
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 0x5F;
    chip8->V[y] = 0xAF;
    arithmetic_2(chip8);
    if (chip8->V[x] == 0x5F & 0xAF) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_arithmetic_3(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY3
        Sets VX to VX (XOR) VY
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 0x5F;
    chip8->V[y] = 0xAF;
    arithmetic_3(chip8);
    if (chip8->V[x] == 0x5F ^ 0xAF) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_arithmetic_4(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY4
        Sets VX to VX + VY, VF = 1 if overflow
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 10;
    chip8->V[y] = 100;
    chip8->V[0xF] = 0;
    arithmetic_4(chip8);
    if (!((chip8->V[x] == 10 + 100) && (chip8->V[0xF] == 0))) {
        return FAILURE;
    }
    chip8->V[x] = 254;
    chip8->V[y] = 250;
    chip8->V[0xF] = 0;
    if ((chip8->V[x] == (254 + 250)) && (chip8->V[0xF] == 1)) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_arithmetic_5(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY5
        Set VX = VX - VY, VF = VX > VY
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 10;
    chip8->V[y] = 11;
    chip8->V[0xF] = 1;
    arithmetic_5(chip8);
    if (!((chip8->V[x] == (unsigned char)(10 - 11)) && (chip8->V[0xF] == 0))) {
        return FAILURE;
    }
    chip8->V[x] = 11;
    chip8->V[y] = 10;
    chip8->V[0xF] = 0;
    arithmetic_5(chip8);
    if ((chip8->V[x] == 11 - 10) && (chip8->V[0xF] == 1)) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_arithmetic_6(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY6
        Set VF = LSB(VX), then shift VX right once
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 10;
    chip8->V[0xF] = 1;
    arithmetic_6(chip8);
    if (!((chip8->V[x] == 10 >> 1) && (chip8->V[0xF] == 0))) {
        return FAILURE;
    }
    chip8->V[x] = 11;
    chip8->V[0xF] = 0;
    arithmetic_6(chip8);
    if ((chip8->V[x] == 11 >> 1) && (chip8->V[0xF] == 1)) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_arithmetic_7(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY7
        Set VX = VY - VX, VF = VY > VX
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 10;
    chip8->V[y] = 11;
    chip8->V[0xF] = 1;
    arithmetic_5(chip8);
    if (!((chip8->V[x] == 11 - 10) && (chip8->V[0xF] == 1))) {
        return FAILURE;
    }
    chip8->V[x] = 11;
    chip8->V[y] = 10;
    chip8->V[0xF] = 0;
    arithmetic_5(chip8);
    if ((chip8->V[x] == (unsigned char)(10 - 11)) && (chip8->V[0xF] == 0)) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_arithmetic_E(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XYE
        Sets VF = MSB(VX), then shifts VX left by 1
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 0x0F;
    chip8->V[0xF] = 1;
    arithmetic_E(chip8);
    if (!((chip8->V[x] == 0x0F << 1) && (chip8->V[0xF] == 0))) {
        return FAILURE;
    }
    chip8->V[x] = 0xF0;
    chip8->V[0xF] = 0;
    arithmetic_E(chip8);
    if ((chip8->V[x] == (unsigned char)(0xF0 << 1)) && (chip8->V[0xF] == 1)) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int illegal_func_int(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY[8, 9, A, B, C, D, F]
    */
    return SUCCESS;
}

int test_0(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x00E0
        Clear the display
      > 0x00EE
        Return from subroutine
      > 0x0NNN
        Jump to machine code routine at NNN. Ignored by modern interpreters
    */

    unsigned short stack_num = 100;

    if (chip8->opcode == 0x00E0) {
        //load some crap into the corners of the display
        memset((void*)chip8->gfx, 1, sizeof(unsigned char) * GFX_WIDTH *
                GFX_HEIGHT);
    }
    else if (chip8->opcode == 0x00EE) {
        //the chip8 should 
        stack_push(&chip8->stack, stack_num);
    }
    else {
        printf("Opcode 0x0NNN not supported.\n");
    }

    //run opcode
    opcode_0(chip8);

    //test
    if (chip8->opcode == 0x00E0) {
        //make sure the gfx have been set to 0
        int i;
        for (i = 0; i < GFX_WIDTH * GFX_HEIGHT; i++) {
            if (chip8->gfx[i]) {
                // fail
                return FAILURE;
            }
        }
    }
    else if (chip8->opcode == 0x00EE) {
        //make sure we did pop something off the stack
        if (chip8->pc != stack_num + 2) {
            // fail
            return FAILURE;
        }
    }
    // success!
    return SUCCESS;
}

int test_1(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x1NNN
        Sets the program counter to NNN.
    */
    unsigned short nnn = (chip8->opcode & 0x0FFF);
    chip8->pc = nnn + 0x1000;
    opcode_1(chip8);
    if (chip8->pc == nnn) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_2(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x2NNN
        Call subroutine at NNN.
    */
    unsigned short nnn = (chip8->opcode & 0x0FFF);
    chip8->pc = nnn + 0x1000;
    opcode_2(chip8);
    if ((chip8->pc == nnn) && (chip8->stack.data[0] == nnn + 0x1000)) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_3(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x3XKK
        Skip the next instruction if VX = KK.
    */
    unsigned char kk = (chip8->opcode & 0x00FF);
    unsigned short x = (chip8->opcode & 0x0F00) >> 8;
    unsigned short old_pc = chip8->pc;
    chip8->V[x] = kk;
    opcode_3(chip8);
    if (chip8->pc != old_pc + 4) {
        return FAILURE;
    }
    chip8->V[x] = kk + 1;
    old_pc = chip8->pc;
    opcode_3(chip8);
    if (chip8->pc != old_pc + 2) {
        return FAILURE;
    }
    return SUCCESS;
}

int test_4(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x4XKK
      Skips the next instruction if VX != KK
    */
    unsigned char kk = (chip8->opcode & 0x00FF);
    unsigned short x = (chip8->opcode & 0x0F00) >> 8;
    unsigned short old_pc = chip8->pc;
    chip8->V[x] = kk;
    opcode_4(chip8);
    if (chip8->pc != old_pc + 2) {
        return FAILURE;
    }
    chip8->V[x] = kk + 1;
    old_pc = chip8->pc;
    opcode_4(chip8);
    if (chip8->pc != old_pc + 4) {
        return FAILURE;
    }
    return SUCCESS;
}

int test_5(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x5XY0
      Skips the next instruction if VX = VY
    */
    unsigned short x = (chip8->opcode & 0x0F00) >> 8;
    unsigned short y = (chip8->opcode & 0x00F0) >> 4;
    unsigned short old_pc = chip8->pc;
    chip8->V[x] = 1;
    chip8->V[y] = 2;
    opcode_5(chip8);
    if (chip8->pc != old_pc + 2) {
        return FAILURE;
    }
    old_pc = chip8->pc;
    chip8->V[x] = 1;
    chip8->V[y] = 1;
    opcode_5(chip8);
    if (chip8->pc != old_pc + 4) {
        return FAILURE;
    }
    return SUCCESS;
}

int test_6(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x6XKK
      Sets VX to KK
    */
    unsigned char kk = (chip8->opcode & 0x00FF);
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    chip8->V[x] = kk + 1;
    opcode_6(chip8);
    if (chip8->V[x] == kk) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_7(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x7XKK
      Set VX = VX + KK
    */
    unsigned char kk = (chip8->opcode & 0x00FF);
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char old_vx = chip8->V[x];
    opcode_7(chip8);
    if (chip8->V[x] = old_vx + kk) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

int test_8(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x8XY?
    */
    return chip8_test_arithmetic[(chip8->opcode & 0x000F)](chip8);
}

int test_9(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0x9XY0
        Skip next instruction if VX != VY
    */
    unsigned short x = (chip8->opcode & 0x0F00) >> 8;
    unsigned short y = (chip8->opcode & 0x00F0) >> 4;
    chip8->V[x] = 1;
    chip8->V[y] = 2;
    chip8->pc = 10;
    opcode_9(chip8);
    if (chip8->pc != 14) {
        return FAILURE;
    }
    chip8->V[x] = 1;
    chip8->V[y] = 1;
    chip8->pc = 10;
    opcode_9(chip8);
    if (chip8->pc != 12) {
        return FAILURE;
    }
    return SUCCESS;
}

int test_A(chip8_t *chip8) {
    /* Tests the following opcodes:
      > 0xANNN
        Sets I to NNN
    */
    unsigned short nnn = (chip8->opcode & 0x0FFF);
    chip8->I = nnn + 1;
    opcode_A(chip8);
    if (chip8->I != nnn) {
        return FAILURE;
    }
    return SUCCESS;
}

int test_B(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0xBNNN
        Jumps to location NNN + V0
    */
    unsigned short nnn = (chip8->opcode & 0x0FFF);
    chip8->V[0] = 1;
    chip8->pc = 0;
    opcode_B(chip8);
    if (chip8->pc != nnn + 1) {
        return FAILURE;
    }
    return SUCCESS;
}

int test_C(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0xCXKK
        Set VX = random byte AND kk
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char kk = (chip8->opcode & 0x00FF);
    chip8->V[x] = 0;
    printf("X: %02X\nKK: %02X\n", x, kk);
    opcode_C(chip8);
    printf("Trial 1:\n\tVX: %02X\n", chip8->V[x]);
    opcode_C(chip8);
    printf("Trial 2:\n\tVX: %02X\n", chip8->V[x]);
    opcode_C(chip8);
    printf("Trial 3:\n\tVX: %02X\n", chip8->V[x]);
    return SUCCESS;
}

int test_D(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0xDXYN
      Draws an N-byte sprite starting at memory location I at (VX, VY)
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    unsigned char y = (chip8->opcode & 0x00F0) >> 4;
    unsigned char n = (chip8->opcode & 0x000F);
    chip8->V[x] = 0;
    chip8->V[y] = 0;
    chip8->I = 0;
    chip8->memory[0] = 1;
    chip8->memory[1] = 1;
    chip8->memory[2] = 0;
    chip8->memory[3] = 0;
    chip8->gfx[0] = 0;
    chip8->gfx[1] = 1;
    chip8->gfx[2] = 0;
    chip8->gfx[3] = 1;
    opcode_D(chip8);
    if (chip8->gfx[0] != 1 ^ 0) {
        return FAILURE;
    }
    if (chip8->gfx[1] != 1 ^ 1) {
        return FAILURE;
    }
    if (chip8->gfx[2] != 0 ^ 0) {
        return FAILURE;
    }
    if (chip8->gfx[3] != 0 ^ 1) {
        return FAILURE;
    }
    if (chip8->V[0xF] = 0) {
        return FAILURE;
    }
    return SUCCESS;
}

int test_E(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0xEX9E
        Skip next instruction if key[VX] is pressed
      > 0xEXA1
        Skip next instruction if key[VX] is not pressed
    */
    unsigned char x = (chip8->opcode & 0x0F00) >> 8;
    if (chip8->opcode & 0x00FF == 0x9E) {
        chip8->V[x] = 0;
        chip8->key[0] = 1;
        chip8->pc = 10;
        opcode_E(chip8);
        if (chip8->pc != 14) {
            return FAILURE;
        }
        chip8->V[x] = 0;
        chip8->key[0] = 0;
        chip8->pc = 10;
        opcode_E(chip8);
        if (chip8->pc != 12) {
            return FAILURE;
        }
    }
    else if (chip8->opcode & 0x00FF == 0xA1) {
        chip8->V[x] = 0;
        chip8->key[0] = 0;
        chip8->pc = 10;
        opcode_E(chip8);
        if (chip8->pc != 14) {
            return FAILURE;
        }
        chip8->V[x] = 0;
        chip8->key[0] = 1;
        chip8->pc = 10;
        opcode_E(chip8);
        if (chip8->pc != 12) {
            return FAILURE;
        } 
    }
    else {
        printf("Something weird happened in test_E.\n");
        return FAILURE;
    }
    return SUCCESS;
}

int test_F(chip8_t *chip8) {
    /*Tests the following opcodes:
      > 0xFX07
        Set VX = delay timer value
      > 0xFX0A
        Wait for a key press, store the value of the key in VX
      > 0xFX15
        Set delay timer = VX
      > 0xFX18
        Set sound timer = VX
      > 0xFX1E
        Set I += VX
      > 0xFX29
        Set I = location of sprite for digit VX
      > 0xFX33
        Store BCD repr of VX in memory locations I, I+1, I+2
      > 0xFX55
        Store registers V0 through VX in memory starting at location I
    */
    unsigned char x = chip8->opcode & 0x0F00 >> 8;
    if (chip8->opcode & 0x00FF == 0x07) {
        chip8->delay_timer = 100;
        chip8->V[x] = 0;
        opcode_F(chip8);
        if (chip8->V[x] != 100) {
            return FAILURE;
        }
    }
    else if (chip8->opcode & 0x00FF == 0x0A) {
        chip8->V[x] = 0;
        chip8->pc = 0;
        opcode_F(chip8);
        if (chip8->pc != 0 || chip8->V[x] != 0) {
            return FAILURE;
        }
        chip8->V[x] = 0;
        chip8->pc = 0;
        chip8->key[0x4] = 1;
        opcode_F(chip8);
        if (chip8->pc == 0 || chip8->V[x] != 0x4) {
            return FAILURE;
        }
    }
    else if (chip8->opcode & 0x00FF == 0x15) {
        chip8->delay_timer = 100;
        chip8->V[x] = 10;
        opcode_F(chip8);
        if (chip8->delay_timer != 10) {
            return FAILURE;
        }
    }
    else if (chip8->opcode & 0x00FF == 0x18) {
        chip8->sound_timer = 100;
        chip8->V[x] = 10;
        opcode_F(chip8);
        if (chip8->sound_timer != 10) {
            return FAILURE;
        }
    }
    else if (chip8->opcode & 0x00FF == 0x1E) {
        chip8->I = 10;
        chip8->V[x] = 10;
        opcode_F(chip8);
        if (chip8->I != 20) {
            return FAILURE;
        }
    }
    else if (chip8->opcode & 0x00FF == 0x29) {
        int i;
        for (i = 0; i <= 0xF; i++) {
            opcode_F(chip8);
            if (chip8->I != i*FONT_HEIGHT) {
                return FAILURE;
            }
        }
    }
    else if (chip8->opcode & 0x00FF == 0x33) {
        chip8->I = 0;
        int hundreds = chip8->V[x] / 100;
        int tens = (chip8->V[x] % 100) / 10;
        int ones = chip8->V[x] % 100 % 10;
        opcode_F(chip8);
        if (chip8->memory[0] != hundreds ||
                chip8->memory[1] != tens ||
                chip8->memory[2] != ones) {
            return FAILURE;
        }
    }
    else if (chip8->opcode & 0x00FF == 0x55) {
        int i;
        chip8->I = 10;
        for (i = 0; i < x; i++) {
            chip8->V[x] = i;
        }
        opcode_F(chip8);
        for (i = 0; i < x; i++) {
            if (chip8->memory[10 + i] != i) {
                return FAILURE;
            }
        }
    }
    else if (chip8->opcode & 0x00FF == 0x65) {
        int i;
        chip8->I = 10;
        for (i = 0; i < x; i++) {
            chip8->memory[10 + i] = i;
        }
        opcode_F(chip8);
        for (i = 0; i < x; i++) {
            if (chip8->V[x] != i) {
                return FAILURE;
            }
        }
    }
    else {
        printf("Something weird happened in test_F.\n");
        return FAILURE;
    }
    return SUCCESS;
}

int run_test(unsigned short test_opcode) {
    chip8_t chip8;
    init_chip8(&chip8);
    chip8.opcode = test_opcode;
    printf("Testing opcode %04x...\n", test_opcode);
    if (test_table[(chip8.opcode >> 12)](&chip8)) {
        printf("Failed.\n");
        return 1;
    }
    else {
        printf("Success!\n");
        return 0;
    }
}

int main(void) {
    char line[5];
    for (;;) {
        printf("Enter opcode: ");
        unsigned int opcode;
        fgets(line, sizeof(line), stdin);
        getchar();
        sscanf(line, "%4x", &opcode);
        run_test(opcode);
    }
    return 0;
}
