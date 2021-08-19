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
#define WINDOW_SCALLING 10

#define FONTSET_SIZE 0x50
#define FONTSET_BYTES_PER_CHAR 5

#define CLOCK_HZ 500
// amount of ns that executing 1 cycle takes
#define CLOCK_HZ_NS ((double)1000000000.0 / (double)CLOCK_HZ)

#define TIMERS_HZ 60
// the time in ns that should pass between each clock update
#define TIMERS_HZ_NS (long)(1000000000.0 / TIMERS_HZ)

typedef struct cpu
{
    uint16_t i;                  // index register(often addressing)
	uint8_t dt;                  // delay timer
    uint8_t st;                  // sound timer
    uint16_t pc;                 // program counter
	uint16_t sp;                 // stack pointer
	uint16_t stack[STACK_SIZE];  // stack itself
	uint8_t regs[16];            // registers 0x0-0xF
} cpu;

// store all the memory related things, like the memory keymap 
// and the screen keymap
typedef struct MemMaps
{
    uint8_t keys[16];                       // keymap
    uint8_t ram[RAM_SIZE];                  // RAM itself

    uint8_t screen[WINDOW_HEIGHT]
                  [WINDOW_WIDTH];          // the screen map. This screen map
                                           // is reduced in size by a factor of
                                           // eight, so each byte represent
                                           // a row of 8 contiguous pixels
} MemMaps;

//******************************************************************************
//* General Functions                                                          *
//******************************************************************************

// initialize memory and registers and load game into memory
void initialize();

// load game into ram
unsigned int load_game(char *game_name, MemMaps *mems);

// return random number between 0-255
uint8_t randnum();

// fetch 2 contigous bytes in memory, starting at pc, and then adds 2 to pc
uint16_t fetch(uint8_t *ram, uint16_t *pc);

// subtract 1 from ST and DT
void timers_tick(cpu *cpuData, struct timespec *timersClock);

// emulate cpu
void emulate(unsigned int game_size, cpu *cpuData, MemMaps *memoryMaps);

// function for handling the cpu clock our emulator cpu
void clock_handler(struct timespec *startTime);

//
#endif
