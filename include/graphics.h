
#include <stdint.h>
#include <SDL2/SDL.h>

// graphics related function declaration

// start SDL2
void init_win(char *game_name[], int width, int height);


// render 1 byte of data at the specified x and y positions
void render(uint8_t data, uint16_t x, uint16_t y);

// reset screen color
void clean_screen();

// return 1 if key is pressed and 0 if not
uint8_t iskeydown(uint8_t key);

// wait for key and return it when found
uint8_t wait_for_key();

// handle events and return the pressed key, key is KEY_NULL(16) if 
// there are no down keys
uint8_t event_loop();