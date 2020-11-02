
#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h> 

#define STACK_SIZE 16
#define MAX_RAM 0xFFF
#define START_ADDRS 0X200

#define GFX_ROWS 64
#define GFX_COLUMNS 32
#define GFX_SIZE GFX_WIDTH * GFX_HEIGHT

#define FONTSET_ADDRESS 0x00
#define FONTSET_BYTES_PER_CHAR 5


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
long load_game(FILE *filep);

// return random number between 0-255
uint8_t randnum();

// emulate cpu
void emulate(long game_size);

#endif