/* 
 * Graphics functions that don't need opcodes to be executed
 * */

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <graphics.h>



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
    KEY_NULL
};


// ******************************************************************************
// *                                 rendering                                  *
// ******************************************************************************

// main sdl structures used by program
SDL_Window *ScreenWindow;
SDL_Surface *ScreenSurface;
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
            fprintf(stderr, "Could not create window: %s", SDL_GetError());
        }

        ScreenSurface = SDL_GetWindowSurface(ScreenWindow);
        if (ScreenSurface == NULL)
        {
            fprintf(stderr, "Could not create window: %s", SDL_GetError());
        }

        ScreenRenderer = SDL_CreateSoftwareRenderer(ScreenSurface);
        if (ScreenSurface == NULL)
        {
            fprintf(stderr, "Could not create window: %s", SDL_GetError());
        }
    }
}



void update_window(uint8_t *gfx[])
{
    SDL_UpdateWindowSurface(ScreenWindow);
}


//******************************************************************************
//*                     key input handling                                     *
//******************************************************************************

uint8_t event_loop()
{
    SDL_Event event;

    uint8_t key;
    if ( SDL_PollEvent(&event) != 0 )
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
                switch ( event.key.keysym.sym )
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
                        key = KEYMAP_TWO;
                        break;
                    case SDLK_z:
                        key = KEYMAP_F;
                        break;
                    case SDLK_x:
                        key = KEYMAP_X;
                        break;
                    case SDLK_c:
                        key = KEYMAP_C;
                        break;
                    case SDLK_v:
                        key = KEYMAP_V;
                        break;
                    default:
                        key = KEYMAP_F;
                }
                break;
            }
            case SDL_KEYUP:
                key = KEY_NULL;
                break;
        }
    }
    return key;
}

uint8_t wait_for_key()
{
    uint8_t eventkey = KEY_NULL;

    // wait until the eventkey is different from KEY_NULL
    while (1)
    {
        eventkey = event_loop();

        if (eventkey != KEY_NULL)
        {
            break;
        }
    }

    return eventkey;
}

uint8_t iskeydown(uint8_t key)
{
    // event key that's is being pressed
    uint8_t eventkey = event_loop();
    uint8_t wait_loops = 0;

    while (eventkey == 16 && wait_loops != 0)
    {
        eventkey = event_loop();
        ++wait_loops;
    }
    

    uint8_t isdown = 0;

    if (eventkey == key && eventkey != 16)
    {
        isdown = 1;
    }

    return isdown;
}
<<<<<<< HEAD
=======
>>>>>>> graphics
