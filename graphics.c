/* 
 * Graphics functions that don't need opcodes to be executed
 * */

#include <SDL2/SDL.h>

#include <graphics.h>

void init_win(char *game_name[], int width, int height)
{
    // our window
    SDL_Window *gwindow = NULL;

    // our surface
    SDL_Surface *msurface = NULL;

    // initialize sdl
    if (SDL_Init( SDL_INIT_VIDEO ) != 0)
    {
        SDL_Log("SDL could not initialize sdl window %s", SDL_GetError());
    } 
    else
    {
        gwindow = SDL_CreateWindow(game_name[0], 100, 100, width, height, \
        SDL_WINDOW_RESIZABLE);

        if (gwindow == NULL)
        {
            SDL_Log("SDL could not create window %s", SDL_GetError());
            
        }
        //SDL_Delay( 2000 );
    }
    
}

