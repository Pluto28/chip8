/* 
 * This file contains the definitions of the functions that represent
 * the opcodes that   
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chip8.h"
#include "opcodes.h"
#include "graphics.h"

//******************************************************************************
//*                             hardware functions                             *
//******************************************************************************
// TODO: maybe rewrite thsese functions and arrays of pointers to functions?
// Not essentially a rewrite, but these names are very ugly, maybe some more
// descriptive names that are more readable

/*// Handle opcodes starting with 0x0
const void (*zeroop[]) (uint16_t opcode, cpu *cpuData, MemMaps *mem) =
{
    cls, cpuNULL, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, ret
};

// Handle opcodes starting with 0x8
const void (*eightop[]) (uint16_t opcode, cpu *cpuData, MemMaps *mem) = 
{
    setvxtovy, vxorvy, vxandvy, vxxorvy, 
    vxaddvy, vxsubvy, shr, 
    vysubvx, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, cpuNULL, cpuNULL, 
    shl
};

// Handle opcodes starting with 0xE
const void (*e_op[]) (uint16_t opcode, cpu *cpuData, MemMaps *mem) = 
{
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, skipifdown, skipnotdown
};

// Handle opcodes starting with 0xF
const void (*special[]) (uint16_t opcode, cpu *cpuData, MemMaps *mem) =
{
    cpuNULL, set_dt, cpuNULL, set_BCD, cpuNULL, 
    reg_dump, reg_load, vx_to_dt, set_st, load_char_addr, 
    vx_to_key, cpuNULL, cpuNULL, cpuNULL, iaddvx, cpuNULL

};

//  The array of pointers to instructions holds pointers to instructions that 
// will be used for calling our implementation of the opcodes for the emulator,
// and call some function in case the first msb nibble(4 bits) isn't
// unique to a specific opcode and need more handling, then the function
// handles it and call other arrays of pointers to functions
static void (*generalop[]) (uint16_t opcode, cpu *cpuData, MemMaps *mem) =
{
    msbis0, jump, call, se, sne, 
    svxevy, setvx, addvx, msbis8, next_if_vx_not_vy, 
    itoa, jmpaddv0, vxandrand, draw, msbise, 
    msbisf
};*/

/*  TODO: Some of the instructions implemented here are not compatible with the
 * S-CHIP implementation, and hence need some tweaks to compute things the way the 
 * program expects, so we should actually make a command-line option for doing
 * that 
 */

// data registers functions

void setvx(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    // set register specified in msb - 4 bits to 
    // the lsb
    uint8_t x = offset2(opcode);
    uint8_t nn = opcode & 0x00ff;

    cpuData->regs[x] = nn;
}

void setvxtovy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t x = offset2(opcode);
    uint8_t y = offset3(opcode);

    cpuData->regs[x] = cpuData->regs[y];
}

void addvx(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{   
    uint8_t nn = opcode & 0x00ff;
    uint8_t x = offset2(opcode);

    // cpuData->regs[x] is VX
    cpuData->regs[x] += nn;
}

void vxaddvy(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    uint8_t *vx = &cpuData->regs[offset2(opcode)];
    uint8_t *vy = &cpuData->regs[offset3(opcode)];

    uint8_t result = *vx + *vy;

    // we probably don't need to perform the second result check, but as 
    // soon as the first one passes, the other shouldn't be performed
    if (((*vx) > 0) && ((*vy) > 0) && (result < (*vx) || result < (*vy))) {
        cpuData->regs[0xf] = 1;
    } else { 
        cpuData->regs[0xf] = 0;
    }

    // change data at the address pointed to by vx. First we get the address at 
    // which vx is stored, then we deference it and store the result of the 
    // addition
    *vx = result;
}

void vxsubvy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t *vx = &cpuData->regs[offset2(opcode)];
    uint8_t *vy = &cpuData->regs[offset3(opcode)];
    
    // If there is a borrow, that is, if vy is bigger than vx for 
    // (vx - vy), then we set the register vf to be NOT borrow.
    //
    // If there is a borrow, vf is set to 0.
    // If there isn't a borrow, vf is set to 1
    if ((*vy) > (*vx)) {
        cpuData->regs[0xf] = 0;
    } else {
        cpuData->regs[0xf] = 1;
    }

    *vx = *vx - *vy;
}

void vysubvx(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{  
    uint8_t *vx = &cpuData->regs[offset2(opcode)];
    uint8_t *vy = &cpuData->regs[offset3(opcode)];

    // If there is a borrow, that is, if vx is bigger than vy for 
    // (vy - vx), then we set the register vf to NOT(borrow). 
    // If there is a borrow, vf is set to 0.
    // If there isn't a borrow, vf is set to 1
    if (*vx > *vy) {
        cpuData->regs[0xf] = 0;
    } else {
        cpuData->regs[0xf] = 1;
    }

    *vx = *vy - *vx;
}

// TODO: change the names of variables and maybe of functions
void vxorvy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t x = offset2(opcode);
    uint8_t y = offset3(opcode);

    cpuData->regs[x] = cpuData->regs[x] | cpuData->regs[y];
}

void vxandvy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t x = offset2(opcode);
    uint8_t y = offset3(opcode);

    cpuData->regs[x] = cpuData->regs[x] & cpuData->regs[y];
}

void vxxorvy(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    uint8_t x = offset2(opcode);
    uint8_t y = offset3(opcode);

    cpuData->regs[x] = cpuData->regs[x] ^ cpuData->regs[y];
}

void shr(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t *vx = &cpuData->regs[offset2(opcode)];

    // set vf to 1 if lsb of vx is 1 and 0 if the lsb is 0
    cpuData->regs[0xf] = *vx & 0x01;

    *vx = *vx >> 1;

    //cpuData->regs[0xf] = cpuData->regs[vy] & 0x01; 
}

// TODO: implementing the s-chip instruction, add command-line option to
// change it if needed
void shl(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t *vx = &cpuData->regs[offset2(opcode)];

    // store msb of vx in vf. 0x80 = 0b10000000
    cpuData->regs[0xf] = (*vx & 0x80) >> 7;

    // shift vx to the right by 1.
    *vx = *vx << 1;
}


// TODO: rewrite when possible
void vxandrand(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t x = offset2(opcode);             // register index
    uint8_t mask = opcode & 0x00ff;  // mask value
    uint8_t rand = randnum();                 // random number

    cpuData->regs[x] = rand & mask;
}


// Flow Control with s

void jump(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{

    uint16_t addr = opcode & 0x0fff;

    // since we add +2 to the cpuData->pc register at the game_loop, 
    // this would jump the instruction at addr before it being executed, so
    // we need to subtract 2 to conform to this behavior
    cpuData->pc = addr;
}

void jmpaddv0(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    // get value to add to v0
    uint16_t nnn = opcode & 0x0fff;

    // get value store at the v0 register
    uint8_t v0 = cpuData->regs[0x0];

    // get the effective address to jump to 
    uint16_t addr = nnn + v0;

    cpuData->pc = addr;
}

// subroutines

void call(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{

    // subtract 1 because of array displacement
    if (cpuData->sp >= (STACK_SIZE - 1)) {
        fprintf(stderr, "chip8: Stack overflow\n");
        exit(1);
    }

    // push the current pc to the stack
    cpuData->stack[cpuData->sp] = cpuData->pc;
    ++cpuData->sp;

    cpuData->pc = (0x0FFF & opcode);
}

void ret(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    // pop the last value stored in the stack
    --cpuData->sp;
    cpuData->pc = cpuData->stack[cpuData->sp];
}
// TODO: implement the 0NNN instruction if needed

// conditional branching using skips


void se(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{   
    uint8_t vx = cpuData->regs[offset2(opcode)];

    uint8_t nn = 0x00FF & opcode;

    // skips next instruction if register vx is equal nn
    if (vx == nn) {
        cpuData->pc += 2;
    }
}

void svxevy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = cpuData->regs[offset2(opcode)];
    uint8_t vy = cpuData->regs[offset3(opcode)];

    if (vx == vy) {
        cpuData->pc += 2;
    }
}

void sne(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = cpuData->regs[offset2(opcode)];
    uint8_t nn = opcode & 0x00FF;

    if (vx != nn) {
        cpuData->pc += 2;
    }
}

void next_if_vx_not_vy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{ 
    uint8_t vx = cpuData->regs[offset2(opcode)];
    uint8_t vy = cpuData->regs[offset3(opcode)];

    if (vx != vy)
    {
        cpuData->pc += 2;
    }
}

// Timers

void set_dt(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = cpuData->regs[offset2(opcode)];
    
    cpuData->dt = vx;
}

void vx_to_dt(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t x = offset2(opcode);

    cpuData->regs[x] = cpuData->dt;
}

void set_st(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = cpuData->regs[offset2(opcode)];

    cpuData->st = vx;
}


// TODO: rewrite this function and the functions handling input on the sdl side

void vx_to_key(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t x = offset2(opcode);
    cpuData->regs[x] = waitkey();
}

void skipifdown(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    uint8_t vx = cpuData->regs[offset2(opcode)]; // value stored in register vx
    
    // 1 if key in vx is pressed, 0 if not
    uint8_t is_pressed = mem->keys[vx];

    // skip if key is pressed
    if (is_pressed) 
    {
        cpuData->pc += 2;
    }
}

void skipnotdown(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = cpuData->regs[offset2(opcode)]; // value stored in register vx

    // 1 if pressed, 0 if not
    uint8_t is_pressed = mem->keys[vx];

    // skip instruction if key is not pressed
    if (!is_pressed) 
    {
        cpuData->pc += 2;
    }
}


// The I Register

void itoa(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint16_t nnn = opcode & 0x0FFF;

    cpuData->i = nnn;
}

void iaddvx(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = cpuData->regs[offset2(opcode)];
    cpuData->i += vx;
}

void draw(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    // where to draw and how many bytes
    uint16_t vx = cpuData->regs[offset2(opcode)];
    uint16_t vy = cpuData->regs[offset3(opcode)];

    // number of rows, in bytes, to write to the screen
    uint8_t rowsb = offset4(opcode);
    printf("Writing %i bytes of data to (%i, %i)\n", rowsb, vx, vy);

    // biti for the index of the bit we are inside the byte, and bytei
    // for how many bytes we have already iterated
    uint16_t biti, bytei;
    
    // byte containing the pixels we will draw
    // and the pixel is the monochromatic pixel to be rendered
    uint8_t pixelb, pixel;
    
    // pointer, in the screen, to the pixel we are gonna render
    uint8_t *screen_pixel;

    cpuData->regs[0xf] = 0;

    // chip8 can draw up to 
    for (bytei = 0; bytei < rowsb; ++bytei)
    {
        pixelb = mem->ram[cpuData->i + bytei];
        
        for (biti = 0; biti < 8; ++biti)
        {
            // separate the bit to be drawn from the byte containing it
            pixel = ((0x80 >> biti)  & pixelb) >> (7 - biti);

            /*  Get the pixel, on the screen, which the draw operation will be
             * performed upon.
             *
             *  The ((bytei * y) % (WINDOW_HEIGHT - 1)) operation tell on which row 
             * position we must operate. Modulus for wrapping
             * 
             *  The ((biti + x) % (WINDOW_WIDTH - 1)) decide on which screen pixel
             * column we should operate. Modulus for wrapping.
             * 
             *  In the operations (WINDOW_WIDTH - 1) and (WINDOW_HEIGHT - 1),
             * the sizes are being subtracted by one because the screen 
             * positions start at 0 and not 1, and this creates a displacement
             * in the maximum (x, y) coordinates of 1 for both x and y, just
             * like what happens with arrays.
             * 
             *  The preceding operations just map a 2d coordinate to a 1d 
             * coordinate for a contigous array in memory
             * 
             *  Adding these 2 to the starting address of the array that maps
             * the screen on memory gives us the address of the pixel to 
             * perform the operation upon
             */
             screen_pixel = &(mem->screen[(bytei + vy) % (WINDOW_HEIGHT)]
                                         [(biti  + vx) % (WINDOW_WIDTH)]);
            
	     if (*screen_pixel && pixel) 
                   cpuData->regs[0xf] = 1;

            *screen_pixel ^= pixel;
        }
    }

    update_window(mem);
}

void cls(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    // set the screen array to 0, theoretically effectively
    memset(mem->screen, 0, WINDOW_WIDTH * WINDOW_HEIGHT);

    clean_screen();
}

// drawing fonts

void load_char_addr(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    // hexadecimal number representing the character to load
    uint8_t hex = cpuData->regs[vx];

    // load the address register to the start address of the character 
    // expected 
    cpuData->i = hex * 5;
}


// Binary-Coded Decimal

void set_BCD(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{

    uint8_t digits[3];
    uint8_t number = cpuData->regs[offset2(opcode)];  // VX

    // store separate digits into the digits array
    for (int i = 2; number > 0; --i)
    {
        digits[i] = number % 10;
        number /= 10;
    }

    // store digits into the ram address starting at I
    memcpy(&mem->ram[cpuData->i], &digits, 3);
}

// register values and memory storage

void reg_dump(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t x = offset2(opcode);
    uint16_t base_addr = cpuData->i;
    int index;

    for (index = 0x0; index <= x; ++index)
    {
        mem->ram[base_addr + index] = cpuData->regs[index];
    }
    
    // TODO: toggle this behavior using command-line options
    // cpuData->i = cpuData->i + cpuData->regs[x] + 1;
}

void reg_load(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t x = offset2(opcode);
    uint16_t base_addr = cpuData->i;
    int index;

    for (index = 0x0; index <= x; ++index)
    {
        cpuData->regs[index] = mem->ram[base_addr + index];
    }

    // TODO: toggle this behavior using command-line options
    // cpuData->i = cpuData->i + cpuData->regs[x] + 1;
}

void cpuNULL(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    fprintf(stderr, "[WARNING] Unknown opcode %#X at %#X\n", opcode,
                     cpuData->pc);
}

