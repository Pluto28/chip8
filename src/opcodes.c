#include <stdint.h>
#include <stdio.h>

#include "chip8.h"

//******************************************************************************
//*                     OcpuData->pcODES FUNCTIONS DEFINITIONS                 *
//******************************************************************************

// call our opcodes according to the function pointers
void msbis0(uint16_t opcode)
{
    if (opcode)
    {
        uint16_t index = offset4(opcode);
        (*zeroop[index]) (opcode);
    }
}

void msbis8(uint16_t opcode)
{
    uint16_t index = offset4(opcode);
    (*eightop[index]) (opcode);
}

void msbise(uint16_t opcode) 
{
    uint16_t index = offset3(opcode);
    (*e_op[index]) (opcode);
}

void msbisf(uint16_t opcode)
{
    uint16_t index = offset4(opcode);
    if (index == 0x5) {
        index = offset3(opcode);
    }

    (*special[index]) (opcode);
}


// Handle opcodes starting with 0x0
void (*zeroop[]) (uint16_t opcode) =
{
    cls, cpuNULL, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, ret
};

// Handle opcodes starting with 0x8
void (*eightop[]) (uint16_t opcode) = 
{
    setvxtovy, vxorvy, vxandvy, vxxorvy, 
    vxaddvy, vxsubvy, lsb_vx_in_vf_r, 
    vysubvx, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, cpuNULL, cpuNULL, 
    svflsl
};

// Handle opcodes starting with 0xE
void (*e_op[]) (uint16_t opcode) = 
{
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, 
    cpuNULL, skipifdown, skipnotdown
};

// Handle opcodes starting with 0xF
void (*special[]) (uint16_t opcode) =
{
    cpuNULL, set_dt, cpuNULL, set_BCD, cpuNULL, 
    reg_dump, reg_load, vx_to_dt, set_st, load_char_addr, 
    vx_to_key, cpuNULL, cpuNULL, cpuNULL, iaddvx, cpuNULL

};

// Call other arrays of functions 
void (*generalop[16]) (uint16_t opcode) =
{
    msbis0, jump, call, se, sne, 
    svxevy, setvx, addvx, msbis8, next_if_vx_not_vy, 
    itoa, jmpaddv0, vxandrand, draw, msbise, 
    msbisf
};


// fuctions for the general function pointers array

void msbis0(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    if (opcode)
    {
        uint16_t index = offset4(opcode);
        (*zeroop[index]) (opcode);
    }
}

void msbis8(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint16_t index = offset4(opcode);
    (*eightop[index]) (opcode);
}

void msbise(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    uint16_t index = offset3(opcode);
    (*e_op[index]) (opcode);
}

void msbisf(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint16_t index = offset4(opcode);
    if (index == 0x5) {
        index = offset3(opcode);
    }

    (*special[index]) (opcode);

}

// data registers functions

void setvx(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    // set register specified in msb - 4 bits to 
    // the lsb
    uint8_t vx = offset2(opcode);
    uint8_t nn = opcode & 0x00ff;

    cpuData->regs[vx] = nn;
}

void setvxtovy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    cpuData->regs[vx] = cpuData->regs[vy];
}

void addvx(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{   
    uint8_t nn = ocpuData->pcode & 0x00ff;
    uint8_t vx = offset2(opcode);

    cpuData->regs[vx] = cpuData->regs[vx] + nn;
}

void vxaddvy(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    //printf("running vxaddvy\n", opcode);
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);
    uint16_t result;

    result = cpuData->regs[vx] + cpuData->regs[vy];

    cpuData->regs[vx] = result;
    uint8_t rflag = result >> 8;

    reg[0xf] = rflag;
}

void vxsubvy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    //printf("running vxsubvy %X\n", opcode);
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    cpuData->regs[vx] = cpuData->regs[vx] - cpuData->regs[vy];
    uint8_t borrow = cpuData->regs[vy] > cpuData->regs[vx];

    reg[0xf] = borrow;
}

void vysubvx(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{  
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    cpuData->regs[vx] = cpuData->regs[vy] - cpuData->regs[vx];
    uint8_t borrow = cpuData->regs[vx] < cpuData->regs[vy];

    reg[0xf] = borrow;
}

void vxorvy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    cpuData->regs[vx] = cpuData->regs[vx] | cpuData->regs[vy];
}

void vxandvy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    cpuData->regs[vx] = cpuData->regs[vx] & cpuData->regs[vy];
}

void vxxorvy(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    cpuData->regs[vx] = cpuData->regs[vx] ^ cpuData->regs[vy];
}

void lsb_vx_in_vf_r(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    reg[0xf] = cpuData->regs[vx] & 0x01;
    cpuData->regs[vx] = cpuData->regs[vx] >> 1;

    //reg[0xf] = cpuData->regs[vy] & 0x01; 
}

void svflsl(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    // store msb of vx in vf
    reg[0xf] = (cpuData->regs[vx] & 0x80) >> 7;

    // shift vy by one bit and store the shifted value in vx
    cpuData->regs[vx] = cpuData->regs[vx] << 1;
}

void vxandrand(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);    // register index

    uint8_t mask = ocpuData->pcode & 0x00ff;  // mask value
    uint8_t rand = randnum();        // random number
    cpuData->regs[vx] = mask & rand;
}


// Flow Control with s

void jump(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{

    uint16_t addr = ocpuData->pcode & 0x0fff;

    // since we add +2 to the cpuData->pc register at the game_loop, 
    // this would jump the instruction at addr before it being executed, so
    // we need to subtract 2 to conform to this behavior
    cpuData->pc = addr - 2;
}

void jmpaddv0(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint16_t nnn = ocpuData->pcode & 0x0fff;

    uint16_t addr = nnn + reg[0x0];
    cpuData->pc = addr - 2;
}

// subroutines

void call(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    //TODO: check if this is behaving the expected way

    // store actual address into stack to return to 
    // it after
    ++SP;
    stack[SP] = cpuData->pc;

    cpuData->pc = (0x0FFF & opcode) - 2;
}

void ret(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    //TODO: check if this is behaving the expected way
    cpuData->pc = stack[SP];
    --SP;
}
// TODO: implement the 0NNN instruction if needed

// conditional branching using skips


void se(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{   
    uint8_t vx = offset2(opcode);

    uint8_t nn = 0x00FF & ocpuData->pcode;

    // skips next instruction if register vx is equal nn
    if (cpuData->regs[vx] == nn) {
        cpuData->pc += 2;
    }
}

void svxevy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);


    if (cpuData->regs[vx] == cpuData->regs[vy]) 
    {
        cpuData->pc += 2;
    }
}

void sne(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t nn = ocpuData->pcode & 0x00FF;

    if (cpuData->regs[vx] != nn) 
    {
        cpuData->pc += 2;
    }
}

void next_if_vx_not_vy(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{ 
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    if (cpuData->regs[vx] != cpuData->regs[vy])
    {
        cpuData->pc += 2;
    }
}

// Timers

void set_dt(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);

    DT = cpuData->regs[vx];
}

void vx_to_dt(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);

    cpuData->regs[vx] = DT;
}

void set_st(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);

    ST = cpuData->regs[vx];
}

// TODO: Keypad Input


void vx_to_key(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    cpuData->regs[vx] = waitkey();
}

void skipifdown(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    uint8_t vx = reg[offset2(opcode)]; // value stored in register vx
    uint8_t is_pressed = keys[vx];     // 1 if key in vx is pressed, 0 otherwise

    // skip if key is pressed
    if (is_pressed) 
    {
        cpuData->pc += 2;
    }
}

void skipnotdown(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = reg[offset2(opcode)]; // value stored in register vx
    uint8_t is_pressed = keys[vx];     // 1 if key in vx is pressed, 0 otherwise

    // skip instruction if key is not pressed
    if (!is_pressed) 
    {
        cpuData->pc += 2;
    }
}


// The I Register
void itoa(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint16_t nnn = ocpuData->pcode & 0x0FFF;

    I = nnn;
}

void iaddvx(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    I = I + cpuData->regs[vx];

}

// drawing sprites to the screen

void draw(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    // where to draw and how many bytes
    uint16_t x = reg[offset2(opcode)];
    uint16_t y = reg[offset3(opcode)];
    uint16_t height = offset4(opcode);

    // byte of data containing the pixel to be written
    uint8_t pixel;
    
    // keep track of the bytei for the y axis and the bit_index for the 
    // index of the bits inside the byte
    uint16_t bytei, bit_index; 
    reg[0xf] = 0;

    for (bytei = 0; bytei < height; ++bytei)
    {
        pixel = rram(bytei + I);

        for (bit_index = 0; bit_index < 8; ++bit_index)
        {
            uint8_t bit = (pixel >> (7 - bit_index)) & 0x1;
            
            // the pixel in the screen
            uint8_t *pixelp = &gfx[y + bytei]
                                  [x + bit_index];


            if (*pixelp == 1 && bit) reg[0xF] = 1;

            *pixelp = *pixelp ^ bit;
        }
    }

    draw_flag = 1;
}

void cls(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint16_t column;
    for (column = 0; column < GFX_COLUMNS; ++column)
    {

        memset(gfx[column], 0, GFX_ROWS);
    }
    draw_flag = 1;
}


// drawing fonts

void load_char_addr(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    // this is the hexadecimal character
    uint8_t hex = reg[offset2(opcode)];

    // set I to the starting address of the data that composes the
    // hexadecimal digit in memory
    I = hex * 5;
}


// Binary-Coded Decimal

void set_BCD(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    int i;

    uint8_t digits[3];
    uint8_t number = reg[offset2(opcode)]; 

    // store separate digits into the digits array
    for (i = 3; number > 0;)
    {
        digits[--i] = number % 10;
        number /= 10;
    }

    // store digits into the ram address starting at I
    memcpy(&ram[I], &digits, 3);
}

// register values and memory storage

void reg_dump(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t index;

    for (index = 0x0; index <= vx; ++index)
    {
        ram[index + I] = reg[index];
    }
    //I = I + vx + 1;
}

void reg_load(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint8_t vx = offset2(opcode);
    uint8_t index;

    for (index = 0x0; index <= vx; ++index)
    {
        reg[index] = ram[I+index];
    }
    //I = I + vx + 1;
}

void cpuNULL(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    fprintf(stderr, "[WARNING] Unknown opcode %#X at %#X\n", opcode, PC);
}