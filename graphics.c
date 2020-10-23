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

uint8_t wait_for_key()
{
    uint8_t key = KEY_NULL;
    SDL_Event event;

    SDL_WaitEvent(&event);

    // get key pressed and if it's not the null key break out of
    // the loops
    key = get_key(&event);
    
    return key;
}

void exit_win()
{
    SDL_DestroyWindow(ScreenWindow);
    SDL_DestroyRenderer(ScreenRenderer);
    exit(0);
}

uint8_t get_key(SDL_Event *event)
{
    uint8_t key = 16;
    switch ( event->key.keysym.sym )
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
            key = KEY_NULL;
    }

    return key;
}

void user_exit()
{
    // event handler
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if ( event.type ==  SDL_QUIT )
        {
            SDL_Quit();
            exit_win();
        }
    }
}

uint8_t iskeydown(uint8_t key)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    // this is the actual pressed key, not confuse with the key passed as
    // argument 
    uint8_t pressed_key = get_key(&event);
    
    // 0 if key is not pressed and 1 if it's, that's the value returned
    uint8_t is_pressed = 0;

    if (event.key.state == SDL_PRESSED && pressed_key == key)
    {
        is_pressed = 1;
    }
    printf("%i\n", pressed_key);
    return is_pressed;
}