#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <features.h>
#include <stdint.h>
#include <sys/random.h>
#include <unistd.h>

#define __USE_GNU
#include <sys/time.h>

#include "chip8.h"
#include "opcodes.h"
#include "graphics.h"

#define rram(addr) (ram[addr])
#define wram(addr, val) (ram[addr] = val)

#define offset1(opcode) ((opcode & 0xF000) >> 12)
#define offset2(opcode) ((opcode & 0x0F00) >> 8)
#define offset3(opcode) ((opcode & 0x00F0) >> 4)
#define offset4(opcode) ((opcode & 0x000F))

//******************************************************************************
// * ARRAYS OF POINTERS TO FUNCTIONS                                           *
//******************************************************************************

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

// fonts
uint8_t fonts[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
    0xF0, 0x80, 0xF0, 0x80, 0x80    // F
};

//******************************************************************************
//*                      general processor functions                           *
//******************************************************************************



int main(int argc, char *argv[])
{
    uint game_size;

    cpu *cpuData;
    MemMaps *mems;
    
    // initialize interpreter and load game into memory
    if (argc == 2) {
        // initialize general variables and arrays to the desired values
        initialize(cpuData, mems);

        // open game and load it in memory
        game_size = load_game(argv[argc-1]);

        // start window using sdl 
        init_win(&argv[argc-1],  GFX_ROWS * 10, GFX_COLUMNS * 10);

        // start cpu emulation
        emulate(game_size, cpuData, mems);
    } else {
        fprintf(stderr, "usage: ./chip8 <game>\n");
        exit(1);
    }

}

uint8_t randnum()
{
    ssize_t buffer[1];
    getrandom(&buffer[0], 1, 0x0);

    // generates number
    srandom(buffer[0]);
    uint8_t number = (random()) % 255;

    return number;
}

void cpuNULL(uint16_t opcode)
{
    fprintf(stderr, "[WARNING] Unknown opcode %#X at %#X\n", opcode, PC);
}

void debug(uint16_t opcode)
{
    printf("\n\nPC: %#X opcode: %#X\n", PC, opcode);
    printf("----------------------------------\n");

    printf("regs:\n");

    uint8_t in;
    for (in = 0; in < sizeof(reg); ++in)
    {
        printf("V%X: %#X    Key%X: %#X\n", in, reg[in],\
                in, keys[in]);
    }

    printf("\n\nSP: %X I: %X", SP, I);
}

// TODO: refactor code to be more efficient
void emulate(uint game_size, cpu *cpuData, MemMaps *memoryMaps)
{
    // when the emulation was started
    struct timeval start_time;

    gettimeofday(&start_time, NULL);

    uint8_t clock;

    for (clock = 0; PC <= (game_size + 0x200); ++clock)
    {

        set_keys(keys);

        cycle();

        // render to screen if draw_flag is set to 1
        if (draw_flag) {
            update_gfx(GFX_COLUMNS, GFX_ROWS, gfx);

            draw_flag = 0;
        }

        
        cpu_tick(cpuData);
    }
}

void clock_handler(struct timeval *start_time)
{

    //TODO: rewrite
    // if the 
}

void cpu_tick(cpu *cpuData)
{
    if (cpuData->dt == 60) {
        cpuData->dt = 0;
    } else {
        (cpuData->dt) += 1;
    }

    if (cpuData->st == 60) {
        cpuData->st = 0;
    } else {
        (cpuData->st) += 1;
    }
}

void cycle()
{

    // store the opcode to be executed
    uint16_t opcode;
    // xor 2 subsequent memory locations to get a 2 bytes opcode
    opcode = (rram(PC) << 8) | rram(PC+1);
    (*generalop[offset1(opcode)]) (opcode);
    
    PC += 2;

    #ifdef DEBUG
        debug(opcode);
    #endif
}


void initialize(cpu *cpuData, MemMaps *mems)
{

    clean_screen();

    // allocate memory for our structures
    cpuData = (cpu *)     malloc(sizeof(cpu));
    mems =    (MemMaps *) malloc(sizeof(MemMaps));
    
    // load fontset
    strcpy((mems->ram), fonts);

    cpuData->i = 0; 
    draw_flag = 0;
    PC = START_ADDRS;
}

uint load_game(char *game_name, MemMaps *mems)
{
    FILE *filep = fopen(game_name, "r");

    if (filep == NULL) {
        perror("chip8: ");
        exit(errno); 
    }


    // read at most 0xdff bytes of data, respecting maximum RAM size 
    // for applications that is specified for chip8
    uint bread = (uint) fread(*(mem->ram + 0x200), sizeof(char), 0xdff, filep);

    if(ferror(filep)) {
        fprintf(stderr, "chip8: error reading file");
        exit(1);
    }
    
    return bread;
}

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