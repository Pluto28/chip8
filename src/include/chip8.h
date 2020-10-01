
#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h> 

#define STACK_SIZE 16
#define MAX_RAM 0xFFF
#define START_ADDRS 0X200

#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define GFX_SIZE GFX_WIDTH * GFX_HEIGHT

#define FONTSET_ADDRESS 0x00
#define FONTSET_BYTES_PER_CHAR 5

uint8_t gfx[GFX_HEIGHT][GFX_WIDTH];


//******************************************************************************
//*                                REGISTERS                                   *
//******************************************************************************

// program counter
uint16_t PC;

// stack pointer
uint16_t SP;

// stack 
uint16_t stack[STACK_SIZE];

// registers V0-VF
uint8_t reg[16];

// array of addressable ram memory
uint8_t ram[MAX_RAM];

// address register
uint16_t I;

// delay timer
uint8_t DT;

// sound timer
uint8_t ST;

// size of file in bytes
long file_size;

//******************************************************************************
//* General Functions                                                          *
//******************************************************************************

// initialize memory and registers and load game into memory
void initialize();

// execute opcodes in RAM linearly
void game_loop();

// load game into ram
void load_game(FILE *filep);

// return random number between 0-255
uint8_t randnum();

#endif