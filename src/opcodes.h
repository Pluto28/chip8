/* 
 * Declaration of functions to execute the respective opcodes
 * */
#pragma once

#include <stdint.h>

#include "chip8.h"

#define offset1(opcode) ((opcode & 0xF000) >> 12)
#define offset2(opcode) ((opcode & 0x0F00) >> 8)
#define offset3(opcode) ((opcode & 0x00F0) >> 4)
#define offset4(opcode) ((opcode & 0x000F))

void cpuNULL(uint16_t opcode, cpu *cpuData, MemMaps *mem);

//******************************************************************************
//*                         FUNCTIONS DECLARATIONS                             *
//******************************************************************************

// TODO: use better names for the functions

// Clears the display. Sets all pixels to off. 00E0
void cls(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Return from subroutine. Set the PC to the address at the top of the stack 
// and subtract 1 from the SP. 00EE
void ret(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// jumps to address NNN. 1NNN
void jump(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// call subroutine at NNN. 2NNN 
void call(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Skips the next instruction if VX equals NN. 3XNN
void se(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Skips the next instruction if VX doesn't equal NN.  4XNN
void sne(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Skips the next instruction if VX equals VY. 5XY0
void svxevy(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets VX to NN. 6XNN
void setvx(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Adds NN to VX. 7XNN
void addvx(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets VX to the value of VY. 8XY0
void setvxtovy(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets VX to VX or VY. 8XY1
void vxorvy(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets VX to VX and VY. 8XY2
void vxandvy(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets VX to VX xor VY. 8XY3
void vxxorvy(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
// 8XY4
void vxaddvy(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// VY is subtracted from VX. VF is set to 0 when there's a borrow,
// and 1 when there isn't. 8XY5
void vxsubvy(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Stores the least significant bit of VX in VF and then shifts
// VX to the right by 1. 8XY6
void lsb_vx_in_vf_r(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when
// there isn't. 8XY7
void vysubvx(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Stores the most significant bit of VX in VF and then shifts
// VX to the left by 1. 8XYE
void svflsl(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Skips the next instruction if VX doesn't equal VY. 9XY0
void next_if_vx_not_vy(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets I to the address NNN. ANNN
void itoa(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Jumps to the address NNN plus V0. BNNN
void jmpaddv0(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets VX to the result of a bitwise and operation on
// a random number (Typically: 0 to 255) and NN. CXNN
void vxandrand(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Draw a sprite at position VX, VY with N bytes of sprite data starting 
// at the address stored in I Set VF to 01 if any set pixels are changed
// to unset, and 00 otherwise
void draw(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Skips the next instruction if the key stored in VX is pressed. EX9E
void skipifdown(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Skips the next instruction if the key stored in VX isn't pressed. EXA1
void skipnotdown(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets VX to the value of the delay timer. FX07
void vx_to_dt(uint16_t opcode, cpu *cpuData, MemMaps *mem);

//  A key press is awaited, and then stored in VX. FX0A
void vx_to_key(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets the delay timer to VX. FX15
void set_dt(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Set the sound timer to the value of register VX. FX18
void set_st(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Adds VX to I. VF is not affected. FX1E
void iaddvx(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Sets I to the location of the sprite for the character in VX. FX29
void load_char_addr(uint16_t opcode, cpu *cpuData, MemMaps *mem);

/* Stores the binary-coded decimal representation of VX, with the most
 * significant of three digits at the address in I, the middle digit at I
 * plus 1, and the least significant digit at I plus 2. (In other 
 * words, take the decimal representation of VX, place the hundreds digit
 *  in memory at location in I, the tens digit at location I+1, and the ones
 *  digit at location I+2.). FX33*/
void set_BCD(uint16_t opcode, cpu *cpuData, MemMaps *mem);
 
// Stores V0 to VX (including VX) in memory starting at address I.
// FX55
void reg_dump(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// Fills V0 to VX (including VX) with values from memory starting at address I.
// FX65
void reg_load(uint16_t opcode, cpu *cpuData, MemMaps *mem);


//******************************************************************************
// * ARRAYS OF POINTERS TO FUNCTIONS                                           *
//******************************************************************************

// call other array of functions if the opcode msb is 0X0,
// lsb is used as index
void msbis0(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// call other array of functions if the opcode msb is 0X8,
// offset3 of opcode is the index
void msbis8(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// call other array of functions if the opcode msb is 0XE,
// 3rd offset of opcode is index
void msbise(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// call other array of functions if the opcode msb is 0XF,
// 4th offset of opcode is index if the 4th offset is not 
// 0X5, if it's 0X5 then the 3rd offset is the index
void msbisf(uint16_t opcode, cpu *cpuData, MemMaps *mem);

// in case opcode is unknown
void cpuNULL(uint16_t opcode, cpu *cpuData, MemMaps *mem);

void (*zeroop[15]) (uint16_t opcode, cpu *cpuData, MemMaps *mem);
void (*eightop[15]) (uint16_t opcode, cpu *cpuData, MemMaps *mem);
void (*e_op[11]) (uint16_t opcode, cpu *cpuData, MemMaps *mem);
void (*special[16]) (uint16_t opcode, cpu *cpuData, MemMaps *mem);
void (*generalop[16]) (uint16_t opcode, cpu *cpuData, MemMaps *mem);