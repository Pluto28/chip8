/* 
 * Graphics functions that don't need opcodes to be executed
 * */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "graphics.h"
#include "chip8.h"


uint8_t sprites[4] = {104, 195, 163, 1};

/*
 * background colors
 *
 * bg[0] = red
 * bg[1] = blue
 * bg[2] = green
 * bg[3] = alpha
 */
uint8_t bg[4] = {0, 0, 0, 255};


// remap the chip8 keys to conform better to new keyboards
// 
// This mapping was originally found at the link:
// http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
// 
// Keypad                   Keyboard
// +-+-+-+-+                +-+-+-+-+
// |1|2|3|C|                |1|2|3|4|
// +-+-+-+-+                +-+-+-+-+
// |4|5|6|D|                |Q|W|E|R|
// +-+-+-+-+       =>       +-+-+-+-+
// |7|8|9|E|                |A|S|D|F|
// +-+-+-+-+                +-+-+-+-+
// |A|0|B|F|                |Z|X|C|V|
// +-+-+-+-+                +-+-+-+-+


enum KeyPressMappings
{
    KEYMAP_X,
    KEYMAP_ONE,
    KEYMAP_TWO,
    KEYMAP_THREE,
    KEYMAP_Q,
    KEYMAP_W,
    KEYMAP_E,
    KEYMAP_A,
    KEYMAP_S,
    KEYMAP_D,
    KEYMAP_Z,
    KEYMAP_C,
    KEYMAP_FOUR,
    KEYMAP_R,
    KEYMAP_F,
    KEYMAP_V,
    KEY_NULL = -1
};


// ******************************************************************************
// *                                 rendering                                  *
// ******************************************************************************

// this struct store pointers to the SDL2 structs that are used to 
// draw and read pixels from the screen
struct WindowDrawData
{

};

// Store colors to be used when drawing pixels or 

// main sdl structures used by program
static SDL_Window *ScreenWindow = NULL;
static SDL_Renderer *ScreenRenderer = NULL;


void init_win(char *game_name, uint8_t scale_factor)
{

    // initialize sdl
    if (SDL_Init( SDL_INIT_VIDEO ) != 0)
    {
        SDL_Log("SDL could not initialize sdl window %s", SDL_GetError());
    } 
    else
    {
        ScreenWindow = SDL_CreateWindow( game_name,
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         WINDOW_WIDTH * scale_factor, 
                                         WINDOW_HEIGHT * scale_factor,
                                         SDL_WINDOW_SHOWN);

        if (ScreenWindow == NULL) {
            fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        }

        ScreenRenderer = SDL_CreateRenderer(ScreenWindow,
                                            -1,
                                            SDL_RENDERER_ACCELERATED);

        if (ScreenRenderer == NULL) {
            fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
    	}
   
	
        SDL_RenderSetScale(ScreenRenderer, scale_factor, scale_factor);
    }
}



void clean_screen()
{   
    SDL_SetRenderDrawColor(ScreenRenderer, 
                           bg[0], bg[1], bg[2], bg[3]);
    SDL_RenderClear(ScreenRenderer);
}

// TODO: add error checking
void update_window(MemMaps *mem)
{
    
    // create texture that will hold the pixels to the screen
    SDL_Texture *ChipTexture = SDL_CreateTexture(ScreenRenderer,
                                                 SDL_PIXELFORMAT_RGBA32,
                                                 SDL_TEXTUREACCESS_STREAMING,
                                                 WINDOW_WIDTH, WINDOW_HEIGHT);

    if (ChipTexture == NULL) {
        fprintf(stderr, "Couldn't create texture from renderer: %s",
                SDL_GetError());
    }

    //--------------------------------------------------------------------------
    // get our texture format and map the rgba colors to it
    uint32_t pixelformat;
    if (SDL_QueryTexture(ChipTexture, &pixelformat, NULL, NULL, NULL) == -1) {
        fprintf(stderr, "Couldn't querry texture format: %s", SDL_GetError());
    }

    SDL_PixelFormat *format = SDL_AllocFormat(pixelformat);
    if (format == NULL) {
        fprintf(stderr, "Couldn't allocate format: %s", SDL_GetError());
    }

    uint32_t spriteRGBA = SDL_MapRGBA(format, 
                                   sprites[0], sprites[1],
                                   sprites[2], sprites[3]);


    uint32_t bgRGBA = SDL_MapRGBA(format, 
                                   bg[0], bg[1],
                                   bg[2], bg[3]);

    //--------------------------------------------------------------------------

    int HeightIndex, WidthIndex;
    uint32_t SurfaceIndex;       // which pixel to access

    // unlock texture so we can manipulate its pixels
    uint32_t *pixels;
    int pitch;
    SDL_LockTexture(ChipTexture, NULL, (void **) &pixels, &pitch);

    // iterate the entire screen array, updates the ChipTexture with 
    // the new frame, then unlocks it and effectivates the changes
    for(HeightIndex = 0; HeightIndex < WINDOW_HEIGHT; ++HeightIndex)
    {
        for(WidthIndex = 0; WidthIndex < WINDOW_WIDTH; ++WidthIndex)
        {
            // set the texture value to a RGBA pixel, according to the
            // memory map that stores the memory map of our screen
            if (mem->screen[HeightIndex][WidthIndex]) {
                pixels[(WINDOW_WIDTH * HeightIndex) + WidthIndex] = spriteRGBA;
            } else {
                pixels[(WINDOW_WIDTH * HeightIndex) + WidthIndex] = bgRGBA;
            }
        }
    }
    SDL_UnlockTexture(ChipTexture);

    SDL_RenderCopy(ScreenRenderer, ChipTexture, NULL, NULL);
    SDL_RenderPresent(ScreenRenderer);
    
    SDL_DestroyTexture(ChipTexture);
}

//******************************************************************************
//*                     key input handling                                     *
//******************************************************************************


uint8_t set_keys(uint8_t *keys)
{
    SDL_Event event;

    //memset(keys, 0, sizeof(uint8_t) * sizeof(keys));
    while (SDL_PollEvent(&event))
    {
        switch ( event.type )
        {
            case SDL_QUIT:
            {
                SDL_Quit();
                exit( 0 );
            }

            case SDL_KEYDOWN: 
            {
               int8_t key = keymap(event.key.keysym.sym);
                if (key >= 0 && key <= 15)
                {
                   keys[key] = 1;
                }
                break;
            }

            case SDL_KEYUP: 
            {
               int8_t key = keymap(event.key.keysym.sym);
               if (key >= 0 && key <= 15)
               {
                    keys[key] = 0;
               }
               break;
            }

        }
    }
}

// TODO: rewrite
uint8_t waitkey()
{
    fprintf(stdout, "\nWaiting for key\n");

    SDL_Event event;
    uint8_t key;

    while (true)
    {
        SDL_WaitEvent(&event);

        if (event.type == SDL_QUIT)
        {
            SDL_Quit();
            exit( 0 );
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            key = keymap(event.key.keysym.sym);

            // break out of loop if key is in the expected range
            if (key >= 0 && key <= 15) {
                break;
            }

        } else {
            continue;
        }
    }

    return key;
}

uint8_t keymap(uint key)
{
    
    switch ( key )
    {
        case SDLK_1:
            key = KEYMAP_ONE;
            break;
        case SDLK_2:
            key = KEYMAP_TWO;
            break;
        case SDLK_3:
            key = KEYMAP_THREE;
            break;
        case SDLK_4:
            key = KEYMAP_FOUR;
            break;
        case SDLK_q:
            key = KEYMAP_Q;
            break;
        case SDLK_w:
            key = KEYMAP_W;
            break;
        case SDLK_e:
            key = KEYMAP_E;
            break;
        case SDLK_r:
            key = KEYMAP_R;
            break;
        case SDLK_a:
            key = KEYMAP_A;
            break;
        case SDLK_s:
            key = KEYMAP_S;
            break;
        case SDLK_d:
            key = KEYMAP_D;
            break;
        case SDLK_f:
            key = KEYMAP_F;
            break;
        case SDLK_z:
            key = KEYMAP_Z;
            break;
        case SDLK_x:
            key = KEYMAP_X;
            break;
        case SDLK_c:
            key = KEYMAP_C;
            break;
        case SDLK_v:
            key = KEYMAP_Z;
            break;
    }

    return key;
}
