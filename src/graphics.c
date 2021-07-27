/* 
 * Graphics functions that don't need opcodes to be executed
 * */

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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
SDL_Window *ScreenWindow;
SDL_Renderer *ScreenRenderer;
SDL_Surface *WindowSurface;


void init_win(char *game_name, uint8_t scale_factor)
{

    // initialize sdl
    if (SDL_Init( SDL_INIT_VIDEO ) != 0)
    {
        SDL_Log("SDL could not initialize sdl window %s", SDL_GetError());
    } 
    else
    {
        ScreenWindow = SDL_CreateWindow(*game_name,
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         WINDOW_WIDTH * scale_factor, 
                                         WINDOW_HEIGHT * scale_factor,
                                         0);

        if (ScreenWindow == NULL) {
            fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        }

        ScreenRenderer = SDL_CreateRenderer(ScreenWindow,
                                            -1,
                                            SDL_RENDERER_ACCELERATED);

        if (ScreenRenderer == NULL) {
            fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        }
        
        WindowSurface = SDL_GetWindowSurface(ScreenWindow);

        if (WindowSurface == NULL) {
            fprintf(stderr, "Could not grab surface: %s\n", SDL_GetError());
        }

        if (SDL_SetSurfaceRLE(WindowSurface, 1) == NULL) {
            fprintf(stderr, "Failed setting surface RLE: %s", SDL_GetError());
        }
    }
}



void clean_screen()
{   
    SDL_SetRenderDrawColor(ScreenRenderer, 
                           bg[0], bg[1], bg[2], bg[3]);
    SDL_RenderClear(ScreenRenderer);
}

// TODO: add error checking
void update_window(uint8_t **screen_map, uint8_t ScaleFactor)
{
    uint32_t BgRGBA = SDL_MapRGBA(WindowSurface->format,
                                   bg[0], bg[1], bg[2], bg[3]);

    uint32_t SpritesRGBA = SDL_MapRGBA(WindowSurface->format,
                                       sprites[0], sprites[1],
                                       sprites[2], sprites[3]);

    // This is the surface that will store the memory map for the emulator 
    // screen
    SDL_Surface *ChipSurface;
    ChipSurface = SDL_CreateRGBSurface(0, 
                                       WindowSurface->w / ScaleFactor,
                                       WindowSurface->h / ScaleFactor,
                                       32,
                                       0, 0, 0, 0); 

    // 
    SDL_LockSurface(WindowSurface);
    SDL_LockSurface(ChipSurface);

    int HeightIndex, WidthIndex;
    uint32_t SurfaceIndex;       // which pixel to access

    for(HeightIndex = 0; HeightIndex <= (WindowSurface->h); ++HeightIndex)
    {
        for(WidthIndex = 0; WidthIndex <= (WindowSurface->pitch); ++WidthIndex)
        {
            // TODO: this should be very inefficient, change it if needed(in
            // case it takes too much time to execute) 
            SurfaceIndex = ((ChipSurface->pitch) * HeightIndex) + WidthIndex;
            // if pixel is set to 1, then the color is for sprites,
            // and if 0, the color is the background color
            if (screen_map[HeightIndex][WidthIndex]) {
                SDL_memset4(ChipSurface->pixels + SurfaceIndex,
                            SpritesRGBA, 1);
            } else {
                SDL_memset4(ChipSurface->pixels + SurfaceIndex,
                            BgRGBA, 1);
            }
        }
    }

    // stretch WindowSurface
    SDL_Rect StretchRect;
    StretchRect.h = WindowSurface->h;
    StretchRect.w = WindowSurface->w;
    StretchRect.x = 0;
    StretchRect.y = 0;
    SDL_BlitScaled(ChipSurface, NULL, WindowSurface, &StretchRect);

    // copy the blitted surface to a texture and then render the texture
    SDL_Texture *TextureToRender;
    TextureToRender = SDL_CreateTextureFromSurface(ScreenRenderer, WindowSurface);
    SDL_RenderCopy(ScreenRenderer, TextureToRender, NULL, NULL);
    SDL_RenderPresent(ScreenRenderer);

    SDL_UnlockSurface(ChipSurface);
    SDL_UnlockSurface(WindowSurface);

    SDL_FreeSurface(ChipSurface);
    SDL_DestroyTexture(TextureToRender);
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

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            key = keymap(event.key.keysym.sym);

            // break out of loop if key is in the expected range
            if (key >= 0 && key <= 15) 
            {
                break;
            }
        }
        else 
        {
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
