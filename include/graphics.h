// graphics related function declaration

// start SDL2
void init_win(char *game_name[], int width, int height);

// wait for key and return it when found
uint8_t wait_for_key();

// update window surface with new gfx
void update_window(uint8_t *gfx[]);

// destroy structs and close down sdl
void exit_win();