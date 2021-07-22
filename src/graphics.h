
#include <stdint.h>
#include <SDL2/SDL.h>

// graphics related function declaration

// start SDL2
void init_win(char *game_name[], int width, int height);

// update surface by using gfx
void update_gfx(uint16_t columns, uint16_t rows, uint8_t gfx[][columns]);

// render 1 byte of data at the specified x and y positions
void render(uint8_t data, uint16_t x, uint16_t y);

// reset screen color
void clean_screen();

// receives a 2d array and a magnifying factor to be used to magnify the passed
// array, returning a pointer to a 2d array that contains the magnified data
uint8_t** magnify(uint16_t h1, uint16_t w1, uint16_t h2, uint16_t w2, uint8_t bitarr[][w1]);


// wait for key and return it when found
uint8_t waitkey();

// handle events and return the pressed key, key is KEY_NULL(16) if 
// there are no down keys
uint8_t set_keys(uint8_t *keys);

uint8_t keymap(uint key);