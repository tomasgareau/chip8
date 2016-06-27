#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "chip8.h"
#include "stack.h"

#define VX chip8->V[(chip8->opcode & 0x0F00) >> 8]
#define VY chip8->V[(chip8->opcode & 0x00F0) >> 4]

/******** GLOBALS ********/

// chip8 fontset
unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/******** CHIP8 CORE ********/


/******** OPCODE FUNCTION POINTERS ********/

void opcode_0(chip8_t *chip8) {
    if (chip8->opcode == 0x00E0) {
        // clear the screen
        memset((void*)chip8->gfx, 0, sizeof(unsigned char) * GFX_WIDTH * GFX_HEIGHT);
        // need to update the screen
        chip8->draw_flag = 1;
        chip8->pc += 2;
    }
    else if (chip8->opcode == 0x00EE) {
        // return from subroutine
        chip8->pc = stack_pop(&chip8->stack);
        chip8->pc += 2;
    }
    else {
        // call RCA 1802 program at address NNN (opcode & 0x0FFF)
        fputs("Opcode error: do not support RCA 1802 programs.\n", stderr);
        opcode_2(chip8);
    }
}

void opcode_1(chip8_t *chip8) {
    // jumps to address NNN (opcode & 0x0FFF)
    chip8->pc = (chip8->opcode & 0x0FFF);
}

void opcode_2(chip8_t *chip8) {
    // calls subroutine at NNN (opcode & 0x0FFF)
    stack_push(&chip8->stack, chip8->pc);
    chip8->pc = (chip8->opcode & 0x0FFF); 
}

void opcode_3(chip8_t *chip8) {
    // skip the next instruction if VX equals NN (0x3XNN)
    if (VX == (chip8->opcode & 0x00FF)) {
        chip8->pc += 4;
    }
    else {
        chip8->pc += 2;
    }
}

void opcode_4(chip8_t *chip8) {
    // skip the next instruction if VX doesn't equal NN (0x4XNN)
    if (VX != (chip8->opcode & 0x00FF)) {
        chip8->pc += 4;
    }
    else {
        chip8->pc += 2;
    }
}

void opcode_5(chip8_t *chip8) {
    // skip the next instruction if VX equals VY (0x5XY0)
    if ((chip8->opcode & 0x000F) == 0) {
        if (VX == VY) {
            chip8->pc += 4;
        }
        else {
            chip8->pc += 2;
        }
    }
}

void opcode_6(chip8_t *chip8) {
    VX = (chip8->opcode & 0x00FF);
    chip8->pc += 2;
}

void opcode_7(chip8_t *chip8) {
    VX += (chip8->opcode & 0x00FF);
    chip8->pc += 2;
}

void opcode_8(chip8_t *chip8) {
    chip8_arithmetic[(chip8->opcode & 0x000F)](chip8);   
}

void opcode_9(chip8_t *chip8) {
    if (!(chip8->opcode & 0x000F)) {
        // skip the next instruction if VX != VY (0x9XY0)
        if (VX != VY) {
            chip8->pc += 4;
        }
        else {
            chip8->pc += 2;
        }
    }
    else {
        illegal_func(chip8);
    }
}

void opcode_A(chip8_t *chip8) {
    // sets I to the address NNN (0xANNN)
    chip8->I = (chip8->opcode & 0x0FFF);
    chip8->pc += 2;
}

void opcode_B(chip8_t *chip8) {
    // jumps to the address NNN plus V0 (0xBNNN)
    chip8->pc = (chip8->opcode & 0x0FFF) + chip8->V[0];
}

void opcode_C(chip8_t *chip8) {
    // sets VX to the result of a bitwise AND on random number & NN (0xCXNN)
    VX = ((rand() & 0xFF) & (chip8->opcode & 0x00FF));
    chip8->pc += 2;
}

void opcode_D(chip8_t *chip8) {
    // display an N-byte sprite starting at memory location I  at (VX, VY)
    // (0xDXYN)
    int x, y;
    unsigned char sprite_row;

    // clear the VF flag - this will be set if there is a collision event
    chip8->V[0xF] = 0;

    for (y = 0; y < (chip8->opcode & 0x000F); y++) {
        // grab the current byte
        sprite_row = chip8->memory[chip8->I + y];
        for (x = 0; x < 8; x++) {
            // check if the current bit is 1 (that's the only time a XOR
            // operation would change the current value on the screen)
            if (sprite_row & (0x80 >> x)) {
                // if the current pixel is 1, a collision has occurred. set VF
                if (chip8->gfx[VX + x + (VY + y)*GFX_WIDTH] == 1) {
                    chip8->V[0xF] = 1;
                }
                // draw the screen
                chip8->gfx[VX + x + (VY + y)*GFX_WIDTH] ^= 1;
            }
        }
    }

    // need to update the screen
    chip8->draw_flag = 1;
    chip8->pc += 2;
}

void opcode_E(chip8_t *chip8) {
    if ((chip8->opcode & 0x00FF) == 0x009E) {
        // skips the next instruction if the key stored in VX is pressed
        if (chip8->key[VX]) {
            chip8->pc += 4;
        }
        else {
            chip8->pc += 2;
        }
    }
    else if ((chip8->opcode & 0x00FF) == 0x00A1) {
        // skips the next instruction if the key stored in VY isn't pressed
        if (chip8->key[VX]) {
            chip8->pc += 2;
        }
        else {
            chip8->pc += 4;
        }
    }
    else {
        illegal_func(chip8);
    }
}

void opcode_F(chip8_t *chip8) {
    int i;
    switch((chip8->opcode & 0x00FF)) {
        case (0x07) :
            // set VX to the value of the delay timer (0xFX07)
            VX = chip8->delay_timer;
            chip8->pc += 2;
            break;
            
        case (0x0A) :
            // a key press is awaited, then stored in VX (0xFX0A)
            ;
            int key_pressed = 0;

            for (i = 0; i < 16; i++) {
                if (chip8->key[i]) {
                    key_pressed = 1;
                    VX = i;
                }
            }

            // if no key has been pressed, do not advance pc. try again next
            // cycle
            if (!key_pressed) {
                return;
            }

            chip8->pc += 2;
            break;

        case (0x15) :
            // sets the delay timer to VX (0xFX15)
            chip8->delay_timer = VX;
            chip8->pc += 2;
            break;

        case (0x18) :
            // sets the sound timer to VX (0xFX18)
            chip8->sound_timer = VX;
            chip8->pc += 2;
            break;

        case (0x1E) :
            // adds VX to I (0xFX1E)
            if (chip8->I > 0xFFF - VX) {
                chip8->V[0xF] = 1;
            }
            else {
                chip8->V[0xF] = 0;
            }
            chip8->I += VX;
            chip8->pc += 2;
            break;

        case (0x29) :
            // sets I to the location of the sprite for the char in VX (0xFX29)
            if (VX > 0xF) {
                fprintf(stderr, "Error: VX holds an illegal character.");
            }
            chip8->I = VX * FONT_HEIGHT;
            chip8->pc += 2;
            break;

        case (0x33) :
            // stores the BCD repr of VX with MSB at addr in I (0xFX33)
            ;
            int val = VX;

            chip8->memory[chip8->I] = val / 100;
            chip8->memory[chip8->I+1] = (val % 100) / 10;
            chip8->memory[chip8->I+2] = val % 100 % 10;

            chip8->pc += 2;
            break;

        case (0x55) :
            // stores V0 to VX (incl. VX) in memory starting at addr I (0xFX55)

            memcpy((void *)&chip8->memory[chip8->I], (void *)chip8->V,
                    sizeof(unsigned char) * ((chip8->opcode & 0x0F00) >> 8));

            chip8->pc += 2;
            break;

        case (0x65) :
            // fills V0 to VX (incl. VX) with vals from memory starting at I
            
            memcpy((void *)chip8->V, (void *)&chip8->memory[chip8->I],
                    sizeof(unsigned char) * ((chip8->opcode & 0x0F00) >> 8));

            chip8->pc += 2;
            break;

        default:
            illegal_func(chip8);
    }
}

void illegal_func(chip8_t *chip8) {
    fprintf(stderr, "Invalid opcode: %X\n", chip8->opcode);
}


void arithmetic_0(chip8_t *chip8) {
    // sets VX to the value of VY (0x8XY0)
    VX = VY;
    chip8->pc += 2;
}

void arithmetic_1(chip8_t *chip8) {
    // sets VX to VX (OR) VY (0x8XY1)
    VX |= VY;
    chip8->pc += 2;
}

void arithmetic_2(chip8_t *chip8) {
    // sets VX to VX (AND) VY (0x8XY2)
    VX &= VY;
    chip8->pc += 2;
}

void arithmetic_3(chip8_t *chip8) {
    // sets VX to VX (XOR) VY (0x8XY3)
    VX ^= VY;
    chip8->pc += 2;
}

void arithmetic_4(chip8_t *chip8) {
    // adds VY to VX. VF is set to 1 when there's a carry, 0 when not
    // (0x8XY4)
    if (VY > 0xFF - VX) {
        chip8->V[0xF] = 1;
    }
    else {
        chip8->V[0xF] = 0;
    }
    VX += VY;
    chip8->pc += 2;
}

void arithmetic_5(chip8_t *chip8) {
    // subtracts VY from VX. VF is set to 0 when there's a borrow, 1 when not
    // (0x8XY5)
    if (VY > VX) {
        chip8->V[0xF] = 1;
    }
    else {
        chip8->V[0xF] = 0;
    }
    VX -= VY;
    chip8->pc += 2;
}

void arithmetic_6(chip8_t *chip8) {
    // shifts VX right by one. VF is set to the value of the LSB of VX before
    // the shift. (0x8XY6)
    chip8->V[0xF] = (VX & 0x1);
    VX >>= 1;
    chip8->pc += 2;
}

void arithmetic_7(chip8_t *chip8) {
    // sets VX to VY minus VX. VF is set to 0 when there's a borrow, 1 when not
    // (0x8XY7)
    if (VX > VY) {
        chip8->V[0xF] = 1;
    }
    else {
        chip8->V[0xF] = 0;
    }
    VX = VY - VX;
    chip8->pc += 2;
}

void arithmetic_E(chip8_t *chip8) {
    // shifts VX left by one. VF is set to the value of the MSB of VX before the
    // shift. (0x8XY7)
    chip8->V[0xF] = (VX & 0x80) >> 7;
    VX <<= 1;
    chip8->pc += 2;
}

void (*opcode_table[16])(chip8_t *chip8) =
{
    opcode_0, opcode_1, opcode_2, opcode_3, opcode_4, opcode_5, opcode_6,
    opcode_7, opcode_8, opcode_9, opcode_A, opcode_B, opcode_C, opcode_D,
    opcode_E, opcode_F
};

void (*chip8_arithmetic[16])(chip8_t *chip8) =
{
    arithmetic_0, arithmetic_1, arithmetic_2, arithmetic_3, arithmetic_4,
    arithmetic_5, arithmetic_6, arithmetic_7, illegal_func, illegal_func,
    illegal_func, illegal_func, illegal_func, illegal_func, arithmetic_E,
    illegal_func
};

/******** EMULATOR FUNCTIONS ********/

int init_chip8(chip8_t *chip8) {
    // initialize the stack
    init_stack(&chip8->stack);

    // zero global variables
    chip8->pc = PROG_MEM_START;
    chip8->opcode = 0;
    chip8->I = 0;
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    chip8->draw_flag = 0;
    
    // set all the keys to 0
    memset((void*)chip8->key, 0, sizeof(unsigned char) * NUM_KEYS);

    // clear the V0-VF registers
    memset((void*)chip8->V, 0, sizeof(unsigned char) * NUM_REGISTERS);

    // clear the screen
    memset((void*)chip8->gfx, 0, sizeof(unsigned char) * GFX_WIDTH * GFX_HEIGHT);
    
    // clear memory
    memset((void*)&chip8->memory[PROG_MEM_START], 0,
            sizeof(unsigned char) * (MEMORY_SZ - PROG_MEM_START));

    // copy fontset into memory
    memcpy((void*)chip8->memory, (void*)fontset, 80);
    return 0;
}

void emulate_cycle(chip8_t *chip8) {
    // fetch next opcode
    chip8->opcode = (chip8->memory[chip8->pc] << 8) | (chip8->memory[chip8->pc+1]);
    fprintf(stderr, "Current opcode: %04X\n", chip8->opcode);
    // execute opcode
    opcode_table[(chip8->opcode & 0xF000) >> 12](chip8);

    // update timers
    if (chip8->delay_timer > 0) {
        chip8->delay_timer--;
    }

    if (chip8->sound_timer > 0) {
        if (chip8->sound_timer == 1) {
            printf("\a\n");
        }
        chip8->sound_timer--;
    }
}

void show_mem(unsigned char *buffer) {
    int i;
    for (i = 0; i < 246/2; i++) {
        printf("%02X%02X\n", buffer[2*i],
                buffer[2*i + 1]);
    }
}

int load_game(chip8_t *chip8, const char *filename) {
    FILE *fp;
    //struct stat st;
    int size;
    int i;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "File error: could not open %s.\n", filename);
        return FILE_OPEN_ERROR;
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    if (size > MEMORY_SZ - PROG_MEM_START) {
        fputs("File error: ROM file too large.", stderr);
        return FILE_SIZE_ERROR;
    }

    unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
    if (buffer == NULL) {
        fputs("Memory error: could not allocate buffer for ROM.", stderr);
        return BUFFER_SIZE_ERROR;
    }

    // read ROM into buffer
    fread(buffer, sizeof(unsigned char), size, (FILE*)fp);

    // copy ROM into memory
    memcpy(&chip8->memory[PROG_MEM_START], buffer, sizeof(unsigned char) * size);
    free(buffer);

    return 0;
}

