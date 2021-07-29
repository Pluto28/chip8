#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <time.h>

#define STACK_SIZE 16

#define RAM_SIZE 0xFFF
#define RAM_END (RAM_SIZE - 1)
#define PROG_RAM_START 0X200

#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32

#define FONTSET_SIZE 0x50
#define FONTSET_BYTES_PER_CHAR 5

#define CLOCK_HZ 500
#define CLOCK_HZ_NS ((double) CLOCK_HZ / (double)1000000000.0)

uint8_t draw_flag;

typedef struct cpu
{
    uint16_t i;                  // index register(often addressing)
	uint8_t dt;                  // delay timer
    uint8_t st;                  // sound timer
    uint16_t pc;                 // program counter
	uint16_t sp;                 // stack pointer
	uint16_t stack[STACK_SIZE];  // stack itself
	uint16_t regs[16];           // registers 0x0-0xF
} cpu;

// store all the memory related things, like the memory keymap 
// and the screen keymap
typedef struct MemMaps
{
    uint8_t keys[16];                      // keymaps
    uint8_t ram[RAM_SIZE];                  // RAM itself

    uint8_t screen[WINDOW_HEIGHT]
                  [WINDOW_WIDTH];     // the screen map. This screen map
                                           // is reduced in size by a factor of
                                           // eight, so each byte represent
                                           // a row of 8 contiguous pixels
} MemMaps;

//******************************************************************************
//* General Functions                                                          *
//******************************************************************************

// initialize memory and registers and load game into memory
void initialize();

// execute opcodes in RAM linearly
void cycle();

// load game into ram
uint load_game(char *game_name, MemMaps *mems);

// return random number between 0-255
uint8_t randnum();

// subtract 1 from ST and DT
void cpu_tick();

// emulate cpu
void emulate(uint game_size, cpu *cpuData, MemMaps *memoryMaps);

// function for handling the cpu clock of our emulator, and since the 
// resolution of the host system is potentially limited, we will just update
// it after 


#endif