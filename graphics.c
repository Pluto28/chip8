/* 
 * Graphics functions that don't need opcodes to be executed
 * */

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <graphics.h>


// the background and foreground colors
uint8_t background[4] = {0, 0, 0, 255};
uint8_t sprites[4] = {255, 255, 255, 255};


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

// main sdl structures used by program
SDL_Window *ScreenWindow;
SDL_Renderer *ScreenRenderer;


void init_win(char *game_name[], int width, int height)
{

    // initialize sdl
    if (SDL_Init( SDL_INIT_VIDEO ) != 0)
    {
        SDL_Log("SDL could not initialize sdl window %s", SDL_GetError());
    } 
    else
    {
        ScreenWindow = SDL_CreateWindow(*game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,\
        width, height, 0);
        if (ScreenWindow == NULL)
        {
            fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        }

        ScreenRenderer = SDL_CreateRenderer(ScreenWindow, -1, SDL_RENDERER_ACCELERATED);
        if (ScreenRenderer == NULL)
        {
            fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        }
    }
}


uint8_t** magnify(uint16_t h1, uint16_t w1, uint16_t h2, uint16_t w2, uint8_t bitarr[][w1])
{

    uint y_ratio = (uint)((w1<<16)/w2) + 1;
    uint x_ratio = (uint)((h1<<16)/h2) + 1;;

    // create a dinamically allocated 2d array
    // the new sizes computed according to the ratio

    uint8_t **magnified = (uint8_t **)malloc(w2 * h2);

    uint16_t actualh; // height being iterated
    for (actualh = 0; actualh < h2; ++actualh)
    {
        magnified[actualh] = (uint8_t*)calloc(w2, sizeof(bitarr[0][0]));
    }

    // resize the bitarray using nearest neighbor scaling
    uint16_t y, x, x2, y2;
    for (y = 0; y < h2; ++y)
    {
        for (x = 0; x < w2; ++x)
        {
            x2 = ((x * x_ratio)>>16) ;
            y2 = ((y * y_ratio)>>16) ;
            magnified[y][x] = bitarr[y2][x2];

        }
    }

    return magnified;

}



void update_gfx(uint16_t columns, uint16_t rows, uint8_t gfx[][columns])
{   

    // magnification ratio
    uint8_t ratio = 10;

    // height and width of magnified 2d array
    uint16_t h2 = (columns * ratio);
    uint16_t w2 = (rows * ratio);

    uint8_t **magnified = magnify(columns, rows, h2, w2 , gfx);

    // iterate a magnified array and render it's contents to the screen
    uint16_t row, column;

    for (column = 0; column < h2; ++column)
    {
        for (row = 0; row < w2; ++row)
        {
            if (magnified[column][row])
            {
                SDL_SetRenderDrawColor(ScreenRenderer, sprites[0], sprites[1],\
                sprites[2], sprites[3]);
            }
            else
            {
                SDL_SetRenderDrawColor(ScreenRenderer, background[0], background[1],\
                background[2], background[3]);
            }
            SDL_RenderDrawPoint(ScreenRenderer, row, column);
            //printf("%i", magnified[column][row]);
        }
        //putchar('\n');
    }
    SDL_RenderPresent(ScreenRenderer);
}

void clean_screen()
{   
    SDL_SetRenderDrawColor(ScreenRenderer, 0, 0, 0, 1);
    SDL_RenderClear(ScreenRenderer);
    SDL_RenderPresent(ScreenRenderer);
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
    fprintf(stdout, "Waiting for key");

    SDL_Event event;
    uint8_t key;

    while (1)
    {
        SDL_WaitEvent(&event);

        key = keymap(event.key.keysym.sym);
        
        if (event.type == SDL_QUIT)
        {
            SDL_Quit();
            exit( 0 );
        }

        // break out of loop if key is in the wanted range
        if (key >= 0 && key <= 15) break;
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
        default:
            key = KEY_NULL;
            break;
    }
    return key;
}
