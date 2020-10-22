/* 
 * Graphics functions that don't need opcodes to be executed
 * */

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

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
    KEYMAP_V
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
        width, height, SDL_WINDOW_INPUT_GRABBED);

        ScreenSurface = SDL_GetWindowSurface(ScreenWindow);

    }
}

void update_window(uint8_t *gfx[])
{
    SDL_UpdateWindowSurface(ScreenWindow);
}

uint8_t wait_for_key()
{
    uint8_t key = 0;

    // exit loop when true
    uint8_t brk = false;

    // event handler
    SDL_Event event;

    while (!brk)
    {
        while( SDL_PollEvent( &event ) != 0 )
        {
            if ( event.type ==  SDL_QUIT )
            {
                exit_win(0);
            }

            switch ( event.key.keysym.sym )
            {
                case SDLK_1:
                    key = KEYMAP_ONE;
                case SDLK_2:
                    key = KEYMAP_TWO;
                case SDLK_3:
                    key = KEYMAP_THREE;
                case SDLK_4:
                    key = KEYMAP_FOUR;
                case SDLK_q:
                    key = KEYMAP_Q;
                case SDLK_w:
                    key = KEYMAP_W;
                case SDLK_e:
                    key = KEYMAP_E;
                case SDLK_r:
                    key = KEYMAP_R;
                case SDLK_a:
                    key = KEYMAP_A;
                case SDLK_s:
                    key = KEYMAP_S;
                case SDLK_d:
                    key = KEYMAP_D;
                case SDLK_f:
                    key = KEYMAP_TWO;
                case SDLK_z:
                    key = KEYMAP_F;
                case SDLK_x:
                    key = KEYMAP_X;
                case SDLK_c:
                    key = KEYMAP_C;
                case SDLK_v:
                    key = KEYMAP_V;
            }
        }
    }
}

void exit_win()
{
    SDL_DestroyWindow(ScreenWindow);
    SDL_DestroyRenderer(ScreenRenderer);
    exit(0);
}