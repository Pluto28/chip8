#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <time.h>

#define STACK_SIZE 16
#define RAM_END 0xFFF
#define START_ADDRS 0X200

#define GFX_ROWS (64 / 8)
#define GFX_COLUMNS (32 / 8)

#define FONTSET_ADDRESS 0x00
#define FONTSET_BYTES_PER_CHAR 5

#define CLOCK_HZ 60
#define CLOCK_RATE_MS ((int) ((1.0 / CLOCK_HZ) * 1000000))

uint8_t draw_flag;

typedef struct cpu
{
    uint16_t i                   // index register(often addressing)
	uint8_t dt;                  // delay timer
    uint8_t st;                  // sound timer
    uint16_t pc;                 // program counter
	uint16_t sp;                 // stack pointer
	uint16_t stack[STACK_SIZE];  // stack itself
	uint16_t regs[16];           // registers 0x0-0xF
}

// store all the memory related things, like the memory keymap 
// and the screen keymap
typedef struct MemMaps
{
    uint8_t keys[16];                      // keymaps
    uint8_t ram[MAX_RAM]                   // RAM itself

    uint8_t screen[GFX_COLUMNS][GFX_ROWS]; // the screen map. This screen map
                                           // is reduced in size by a factor of
                                           // eight, so each byte represent
                                           // a row of 8 contiguous pixels
}

//******************************************************************************
//* General Functions                                                          *
//******************************************************************************

// initialize memory and registers and load game into memory
void initialize();

// execute opcodes in RAM linearly
void cycle();

// load game into ram
long load_game(char game_name[]);

// return random number between 0-255
uint8_t randnum();

// subtract 1 from ST and DT
void cpu_tick();

// emulate cpu
void emulate(long game_size);

// responsible for handling the clock difference between the starting
// and ending time and sleeping if the elapsed time is smaller than 
// one second
void clock_handler(struct timeval *now_time);

#endif