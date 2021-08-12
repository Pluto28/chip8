#include <errno.h>
#include <SDL2/SDL.h>
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

//******************************************************************************
// * ARRAYS OF POINTERS TO FUNCTIONS                                           *
//******************************************************************************

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
const void (*generalop[]) (uint16_t opcode, cpu *cpuData, MemMaps *mem) =
{
    msbis0, jump, call, se, sne, 
    svxevy, setvx, addvx, msbis8, next_if_vx_not_vy, 
    itoa, jmpaddv0, vxandrand, draw, msbise, 
    msbisf
};

// call our opcodes according to the function pointers
void msbis0(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    if (opcode)
    {
        uint16_t index = offset4(opcode);
        (*zeroop[index]) (opcode, cpuData, mem);
    }
}

void msbis8(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint16_t index = offset4(opcode);
    (*eightop[index]) (opcode, cpuData, mem);
}

void msbise(uint16_t opcode, cpu *cpuData, MemMaps *mem) 
{
    uint16_t index = offset3(opcode);
    (*e_op[index]) (opcode, cpuData, mem);
}

void msbisf(uint16_t opcode, cpu *cpuData, MemMaps *mem)
{
    uint16_t index = offset4(opcode);
    if (index == 0x5) {
        index = offset3(opcode);
    }

    (*special[index]) (opcode, cpuData, mem);
}

//-----------------------------------------------------------------------------

// fonts
static uint8_t fonts[80] = {
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

    
    // initialize interpreter and load game into memory
    if (argc == 2) {
        cpu cpuData;
        MemMaps mems;
        
        // initialize general variables and arrays to the desired values
        initialize(&cpuData, &mems);

        // open game and load it in memory
        game_size = load_game(argv[argc-1], &mems);

	    char *game_name = argv[1];
        // start window using sdl 
        init_win(game_name, WINDOW_SCALLING);

        // start cpu emulation
        emulate(game_size, &cpuData, &mems);
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


/*void debug(uint16_t opcode, cpu *cpuData, MemMaps *mem)
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

// TODO: refactor code to be more efficient
void emulate(uint game_size, cpu *cpuData, MemMaps *memoryMaps)
{
    uint16_t opcode;

    // start time is the time at which the executing cycle was started
    struct timespec startTime, timersClock;
    clock_gettime(CLOCK_MONOTONIC, &timersClock);

    for (;(cpuData->pc) <= (game_size + 0x200); )
    {
        clock_gettime(CLOCK_MONOTONIC, &startTime);
        set_keys(memoryMaps->keys);

        opcode = fetch(memoryMaps->ram, &cpuData->pc);

        // execute opcode
        (generalop[(opcode & 0xF000) >> 12]) (opcode, cpuData, memoryMaps);
        
        clock_handler(&startTime);
        timers_tick(cpuData, &timersClock);
    }
}

void clock_handler(struct timespec *startTime)
{

    
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
    if (CycleExec_ns < CLOCK_HZ_NS) {
        struct timespec sleeptime;
        sleeptime.tv_nsec = CLOCK_HZ_NS - CycleExec_ns;
        sleeptime.tv_sec = 0;

        if(clock_nanosleep(CLOCK_MONOTONIC, 0, &sleeptime, NULL) == -1) {
            perror("chip8: ");
        }
    }
}

void timers_tick(cpu *cpuData, struct timespec *timersClock)
{

    struct timespec nowtime;
    clock_gettime(CLOCK_MONOTONIC, &nowtime);

    // get the difference between the last update of the clocks
    // and now, and if the difference is bigger or equal to the 
    // delay between clock updates, we update the clocks and the 
    // timer
    long diffNs = nowtime.tv_nsec - timersClock->tv_nsec;
    if (diffNs >= TIMERS_HZ_NS) {
        
        if (cpuData->dt > 0) {
            cpuData->dt -= 1;
        }

        if (cpuData->st > 0) {
            // TODO: call function to play that good ol jazz.
            cpuData->st -= 1;
        }

        clock_gettime(CLOCK_MONOTONIC, timersClock);
    }
}

uint16_t fetch(uint8_t *ram, uint16_t *pc)
{
    uint16_t opcode;

    // get the glorious opcode representing the function to perform in 
    // binary
    opcode = (ram[*pc] << 8) | ram[*pc + 1];

    *pc += 2;

    return opcode;
}

void initialize(cpu *cpuData, MemMaps *mems)
{
    explicit_bzero(mems->screen, WINDOW_HEIGHT * WINDOW_WIDTH);

    // Can you smell that? Yes, my friend, that is the smell of sanitizer
    explicit_bzero(mems->ram, 
                   (RAM_END-1) * sizeof(mems->ram[0]));
    
    // load fontset
    memcpy(mems->ram, fonts, (FONTSET_SIZE - 1));

    // set some default values
    cpuData->i = 0;
    cpuData->pc = PROG_RAM_START;
    cpuData->st = 60;
    cpuData->dt = 60;
}

uint load_game(char *game_name, MemMaps *mems)
{
    FILE *filep = fopen(game_name, "r");

    if (filep == NULL) {
        perror("chip8: ");
        exit(errno); 
    }


    // read at most 0xdff bytes of data, respecting the maximum amount of
    // ram that is available to store applications, according to the chip8
    // specification
    uint bread = (uint) fread((mems->ram + 0x200),
                              sizeof(char),
                              RAM_SIZE - PROG_RAM_START,
                              filep);

    if(ferror(filep)) {
        fprintf(stderr, "chip8: error reading file");
        exit(1);
    }
    
    return bread;
}
