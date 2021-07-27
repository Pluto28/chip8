#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <features.h>
#include <stdint.h>
#include <sys/random.h>
#include <unistd.h>
#include <time.h>

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
        game_size = load_game(argv[argc-1], mems);

        // start window using sdl 
        init_win(&argv[argc-1], 1);

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

/*void debug(uint16_t opcode)
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
}*/

// TODO: rewrite this function
void emulate(uint game_size, cpu *cpuData, MemMaps *memoryMaps)
{
    struct timespec nsResMonotonic;
    struct timespec startTime, timersClock;
    
    clock_getres(CLOCK_MONOTONIC, &nsResMonotonic);
    clock_gettime(CLOCK_MONOTONIC, &timersClock);

    for (;(cpuData->pc) <= (game_size + 0x200); )
    {
        clock_gettime(CLOCK_MONOTONIC, &startTime);
        set_keys(memoryMaps->keys);

        cycle();

        // render to screen if draw_flag is set to 1
        if (draw_flag) {
            // TODO: call drawing function

            draw_flag = 0;
        }
        cpu_tick()
        
        cpu_tick(cpuData);
    }
}

<<<<<<< HEAD
void clock_handler(struct timespec *startTime, struct timespec *MonotonicRes)
=======
void cpu_clock(struct timeval *start_time)
>>>>>>> 160e86b6c26a5c70f9880523be8906d91fd85c3a
{
    // TODO: we should probably move this to a structure encapsulating all 
    // details that are emulation specific

<<<<<<< HEAD
    // amount of ns that executing 1 cycle takes
    long cycle_ns = 1000000000 / CLOCK_HZ;
    
    // TODO: we should check if the resolution is high enough to handle 
    // the amount of nanoseconds that one cycle takes, and if not, we should 
    // increase the amount of cycles to execute before calling this function
    // (one cycle per call should yield better resolution though)

    // we sleep the difference between the time it takes to execute a cycle in 
    // nanoseconds and the time it took to execute the cycle
    struct timespec timenow;
    clock_gettime(CLOCK_MONOTONIC, &timenow);

    // Amount of time the cycle took to execute
    long CycleExec_ns = timenow.tv_nsec - startTime->tv_nsec;

    // in case the cycle took more than or the exact amount of time
    // it should take to execute, we don't need to sleep and can just 
    // end the function
    if (CycleExec_ns < cycle_ns) {
        struct timespec sleeptime;
        sleeptime.tv_nsec = cycle_ns - CycleExec_ns;
        sleeptime.tv_sec = 0;

        if(clock_nanosleep(CLOCK_MONOTONIC, 0, &sleeptime, NULL) == -1) {
            perror("chip8: ");
        }
    }
=======
     
>>>>>>> 160e86b6c26a5c70f9880523be8906d91fd85c3a
}

void cpu_tick(cpu *cpuData, struct timespec *timersClock)
{
    // TODO: store this value somewhere else, since it can just be set 
    // at the start of the program and never more

    // the time in ns that should pass between each clock update
    long clock_ns = 1000000000 / 60;

    struct timespec nowtime;
    clock_gettime(CLOCK_MONOTONIC, &nowtime);

    long diffNs = nowtime.tv_nsec - timersClock->tv_nsec;
    if (diffNs >= clock_ns) {
        
        if (cpuData->dt > 0) {
            cpuData->dt -= 1;
        }

        if (cpuData->st > 0) {
            // TODO: call function to play that good ol jazz.
            cpuData->st -= 1;
        }
    }
}

void cycle()
{
    uint16_t opcode;
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
    cpuData->pc = START_ADDRS;
    draw_flag = 0;
}

uint load_game(char *game_name, MemMaps *mems)
{
    FILE *filep = fopen(game_name, "r");

    if (filep == NULL) {
        perror("chip8: ");
        exit(errno); 
    }


    // read at most 0xdff bytes of data, respecting the maximum RAM size 
    // for applications that is specified for chip8
    uint bread = (uint) fread((mems->ram + 0x200),
                              sizeof(char),
                              0xdff,
                              filep);

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