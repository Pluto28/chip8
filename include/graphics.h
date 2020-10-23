#include <stdint.h>
#include <SDL2/SDL.h>

// graphics related function declaration

// start SDL2
void init_win(char *game_name[], int width, int height);

// wait for key and return it when found
uint8_t wait_for_key();

// update window surface with new gfx
void update_window(uint8_t *gfx[]);

// close sdl and exit program
void exit_win();

// return key number between 0-16. 16 means no key pressed
uint8_t get_key(SDL_Event *event);

// close if user clicked in the exit button
void user_exit();

// render 1 byte of data at the specified x and y positions
void render(uint8_t sprite);

// return 1 if key is pressed and 0 if not
uint8_t iskeydown(uint8_t key);
