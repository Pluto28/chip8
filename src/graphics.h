
#include <stdint.h>
#include <SDL2/SDL.h>


// start SDL2
void init_win(char *game_name, uint8_t scale_factor);

/* 
 * Draw the screen memory map to our screen. There are 3 steps to it:
 * step 1: first we translate the screen map to a surface SDL2 structure
 * step 2: on this step, we blit the surface and then convert it to a texture
 * step 3: we pass the data from the texture to the renderer and then render
 * it
 */ 
void update_window();

// render 1 byte of data at the specified x and y positions
void render(uint8_t data, uint16_t x, uint16_t y);

// reset screen color
void clean_screen();

// wait for key and return it when found
uint8_t waitkey(uint8_t **screen_map, uint8_t ScaleFactor);

// handle events and return the pressed key, key is KEY_NULL(16) if 
// there are no down keys
uint8_t set_keys(uint8_t *keys);

uint8_t keymap(uint key);