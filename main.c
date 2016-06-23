#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"
#include "SDL/SDL.h"

#define THOUSAND 1000L

extern int draw_flag;

int main(int argc, char **argv) {
    srand(time(NULL));

    /******** SDL initialization ********/
    SDL_Surface *screen = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_SetVideoMode(GFX_WIDTH * SCALE, GFX_HEIGHT * SCALE,
            32, SDL_SWSURFACE | SDL_DOUBLEBUF);
    SDL_Event event;
    int quit = 0;

    unsigned char old_gfx[GFX_WIDTH * GFX_HEIGHT];

    /******** Chip8 initialization ********/

    chip8_t chip8;

    if (init_chip8(&chip8)) {
        fputs("Could not initialize chip8.\n", stderr);
        return EXIT_FAILURE;
    }

    memcpy(old_gfx, chip8.gfx, GFX_WIDTH * GFX_HEIGHT);

    if (load_game(&chip8, argv[1])) {
        fputs("Could not load game.\n", stderr);
        return EXIT_FAILURE;
    }

    SDL_WM_SetCaption("chip8", 0);

    while (!quit) {

        emulate_cycle(&chip8);
        if (draw_flag) {
            SDL_Rect rect;
            rect.x = 0;
            rect.y = 0;
            rect.h = GFX_WIDTH * SCALE;
            rect.w = GFX_WIDTH * SCALE;

            for (int j = 0; j < 32; j++) {
                for (int i = 0; i < 64; i++) {
                    if (chip8.gfx[i + j*64] ^ old_gfx[i + j*64] == 1) {
                        rect.x = i*SCALE;
                        rect.y = j*SCALE;
                        rect.w = SCALE;
                        rect.h = SCALE;
                        if (chip8.gfx[i + j*64] == 1) {
                            SDL_FillRect(screen, &rect,
                                    SDL_MapRGB(screen->format, 255, 255, 255));
                        }
                        if (chip8.gfx[i + j*64] == 0) {
                            SDL_FillRect(screen, &rect,
                                    SDL_MapRGB(screen->format, 0, 0, 0));
                        }
                        old_gfx[i + j*64] = chip8.gfx[i + j*64];
                    }
                }
            }
            SDL_Flip(screen);
            draw_flag = 0;
        }

        while (SDL_PollEvent (&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
						case SDLK_1: chip8.key[0x1] = 1; break;
						case SDLK_2: chip8.key[0x2] = 1; break;
						case SDLK_3: chip8.key[0x3] = 1; break;
						case SDLK_4: chip8.key[0xC] = 1; break;
						case SDLK_q: chip8.key[0x4] = 1; break;
						case SDLK_w: chip8.key[0x5] = 1; break;
						case SDLK_e: chip8.key[0x6] = 1; break;
						case SDLK_r: chip8.key[0xD] = 1; break;
						case SDLK_a: chip8.key[0x7] = 1; break;
						case SDLK_s: chip8.key[0x8] = 1; break;
						case SDLK_d: chip8.key[0x9] = 1; break;
						case SDLK_f: chip8.key[0xE] = 1; break;
						case SDLK_z: chip8.key[0xA] = 1; break;
						case SDLK_x: chip8.key[0x0] = 1; break;
						case SDLK_c: chip8.key[0xB] = 1; break;
						case SDLK_v: chip8.key[0xF] = 1; break;
						case SDLK_ESCAPE: exit(1); break;
				}
			} 
            else if (event.type == SDL_KEYUP) {
				switch(event.key.keysym.sym) {
						case SDLK_1: chip8.key[0x1] = 0; break;
						case SDLK_2: chip8.key[0x2] = 0; break;
						case SDLK_3: chip8.key[0x3] = 0; break;
						case SDLK_4: chip8.key[0xC] = 0; break;
						case SDLK_q: chip8.key[0x4] = 0; break;
						case SDLK_w: chip8.key[0x5] = 0; break;
						case SDLK_e: chip8.key[0x6] = 0; break;
						case SDLK_r: chip8.key[0xD] = 0; break;
						case SDLK_a: chip8.key[0x7] = 0; break;
						case SDLK_s: chip8.key[0x8] = 0; break;
						case SDLK_d: chip8.key[0x9] = 0; break;
						case SDLK_f: chip8.key[0xE] = 0; break;
						case SDLK_z: chip8.key[0xA] = 0; break;
						case SDLK_x: chip8.key[0x0] = 0; break;
						case SDLK_c: chip8.key[0xB] = 0; break;
						case SDLK_v: chip8.key[0xF] = 0; break;
				}
			} 
            else if (event.type == SDL_QUIT) {
				quit = 1;
            }
        }

        SDL_Delay(1);
    }

    return EXIT_SUCCESS;
}
