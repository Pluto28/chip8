
#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <time.h>


#define STACK_SIZE 16
#define MAX_RAM 0xFFF
#define START_ADDRS 0X200

#define GFX_ROWS 64
#define GFX_COLUMNS 32

#define FONTSET_ADDRESS 0x00
#define FONTSET_BYTES_PER_CHAR 5

#define CLOCK_HZ 60
#define CLOCK_RATE_NS ((int) ((1.0 / CLOCK_HZ) * 1000000))

#define CPU_TICKS ((int) ())

uint8_t draw_flag;

uint8_t gfx[GFX_COLUMNS][GFX_ROWS];

uint16_t I;
// program counter
uint16_t PC = START_ADDRS;

uint16_t SP;
uint16_t stack[STACK_SIZE];

uint8_t reg[16];
int8_t keys[16];

uint8_t ram[MAX_RAM];

uint8_t DT;
uint8_t ST;


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

void clock_handler(struct timespec *now_time);

#endif