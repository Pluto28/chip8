#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <features.h>
#include <stdint.h>
#include <sys/random.h>
#include <unistd.h>

#define __USE_GNU
#include <sys/time.h>

#include <chip8.h>
#include <opcodes.h>
#include <graphics.h>

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
    long game_size;
    // initialize interpreter and load game into memory
    if (argc == 2)
    {
        // initialize general variables and arrays to the desired values
        initialize();

        // open game and load it in memory
        game_size = load_game(argv[argc-1]);

        // start window using sdl 
        init_win(&argv[argc-1],  GFX_ROWS * 10, GFX_COLUMNS * 10);

        // start cpu emulation
        emulate(game_size);
    }
    else 
    {
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
void emulate(long game_size)
{
    // when the emulation was started
    struct timeval start_time;

    gettimeofday(&start_time, NULL);

    uint8_t clock;

    for (clock = 0; PC <= (game_size + 0x200); ++clock)
    {

        set_keys(keys);

        cycle();

        // render to screen is draw_flag is set to 1
        if (draw_flag)
        {
            update_gfx(GFX_COLUMNS, GFX_ROWS, gfx);

            draw_flag = 0;
        }

        // true if we executed the amount of cycles per second
        // defined in CLOCK_HZ
        if (clock == CLOCK_HZ)
        {
            cpu_tick();
            clock_handler(&start_time);
            clock = 0;
        }
    }
}

void clock_handler(struct timeval *start_time)
{
    struct timeval end_time = {0, 0};
    struct timeval difftime = {0, 0};
    struct timeval clock_rate = {0, CLOCK_RATE_MS};

    // nonzero if the operation end_time-start_time is
    // smaller than CLOCK_RATE_MS
    uint8_t diff_smaller_rate;

    // Time of end
    gettimeofday(&end_time, NULL);
    
    // subtract start_time from end_time and store the result in difftime
    timersub(&end_time, start_time, &difftime);

    diff_smaller_rate = timercmp(&difftime, &clock_rate, <);

    // if the difference between the start and ending time is smaller than the
    // amount of nanoseconds that 60 clocks take, then we need to sleep the
    // difference
    if (diff_smaller_rate)
    {
        // get the amount of time we need to sleep in nanoseconds
        struct timeval sleep_ns;
        // subtract difftime from clock_rate to get the amount of time to sleep
        timersub(&clock_rate, &difftime, &sleep_ns);

        // convert the sleep_ns timeval structure to a timespec structure
        struct timespec sleep_time;
        TIMEVAL_TO_TIMESPEC(&sleep_ns, &sleep_time);

        nanosleep(&sleep_time, NULL);
    }

    // update start_time to the ending time of the last cycle
    start_time->tv_sec = end_time.tv_sec;
    start_time->tv_usec = end_time.tv_usec;

}

void cpu_tick()
{
    if(DT)
    {
        --DT;
    }
    else 
    {
        DT = 60;
    }

    if (ST)
    {
        --ST;
    }
    else 
    {
        ST = 60;
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


void initialize()
{

    clean_screen();

    memset(&stack,  0, sizeof(stack) / sizeof(stack[0]));
    memset(&reg,    0,   sizeof(reg));
    memset(&ram,    0,   sizeof(ram));
    memset(&keys,   0,  sizeof(uint8_t) / sizeof(keys[0]));

    // load fontset
    strcpy(ram, fonts);

    SP = 0;
    I = 0;
    ST = 60; 
    draw_flag = 0;
    PC = START_ADDRS;

}

long load_game(char game_name[])
{
    FILE *filep;

    // check if file exists
    if (access(game_name, R_OK | F_OK) == 0)
    {
        filep = fopen(game_name, "rb");
    }
    else
    {
        fprintf(stderr, "[ERROR] File coudn't be accessed\n");
        exit( 0 );
    }

    // check for errors 
    int stream_status = ferror(filep);
    if (stream_status != 0) {
        fprintf(stderr, "[ERROR] Error reading file, exiting");
        exit(stream_status);
    }

    long file_size;
    fseek(filep, 1, SEEK_END);
    file_size = ftell(filep);
    rewind(filep);


    // load ram with file contents
    fread(&ram[START_ADDRS], 1, sizeof(ram), filep);

    fclose(filep);
    fprintf(stdout, "[OK] successfully loaded file into memory\n");

    return file_size;
}


//******************************************************************************
//*                     OPCODES FUNCTIONS DEFINITIONS                          *
//******************************************************************************

// fuctions for the general function pointers array

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

// data registers functions

void setvx(uint16_t opcode) 
{
    // set register specified in msb - 4 bits to 
    // the lsb
    uint8_t vx = offset2(opcode);
    uint8_t nn = opcode & 0x00ff;

    reg[vx] = nn;
}

void setvxtovy(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    reg[vx] = reg[vy];
}

void addvx(uint16_t opcode)
{   
    uint8_t nn = opcode & 0x00ff;
    uint8_t vx = offset2(opcode);

    reg[vx] = reg[vx] + nn;
}

void vxaddvy(uint16_t opcode) 
{
    //printf("running vxaddvy\n", opcode);
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);
    uint16_t result;

    result = reg[vx] + reg[vy];

    reg[vx] = result;
    uint8_t rflag = result >> 8;

    reg[0xf] = rflag;
}

void vxsubvy(uint16_t opcode)
{
    //printf("running vxsubvy %X\n", opcode);
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    reg[vx] = reg[vx] - reg[vy];
    uint8_t borrow = reg[vy] > reg[vx];

    reg[0xf] = borrow;
}

void vysubvx(uint16_t opcode)
{  
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    reg[vx] = reg[vy] - reg[vx];
    uint8_t borrow = reg[vx] < reg[vy];

    reg[0xf] = borrow;
}

void vxorvy(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    reg[vx] = reg[vx] | reg[vy];
}

void vxandvy(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    reg[vx] = reg[vx] & reg[vy];
}

void vxxorvy(uint16_t opcode) 
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    reg[vx] = reg[vx] ^ reg[vy];
}

void lsb_vx_in_vf_r(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    reg[0xf] = reg[vx] & 0x01;
    reg[vx] = reg[vx] >> 1;

    //reg[0xf] = reg[vy] & 0x01; 
}

void svflsl(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    // store msb of vx in vf
    reg[0xf] = (reg[vx] & 0x80) >> 7;

    // shift vy by one bit and store the shifted value in vx
    reg[vx] = reg[vx] << 1;
}

void vxandrand(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);    // register index

    uint8_t mask = opcode & 0x00ff;  // mask value
    uint8_t rand = randnum();        // random number
    reg[vx] = mask & rand;
}


// Flow Control with s

void jump(uint16_t opcode)
{

    uint16_t addr = opcode & 0x0fff;

    // since we add +2 to the PC register at the game_loop, 
    // this would jump the instruction at addr before it being executed, so
    // we need to subtract 2 to conform to this behavior
    PC = addr - 2;
}

void jmpaddv0(uint16_t opcode)
{
    uint16_t nnn = opcode & 0x0fff;

    uint16_t addr = nnn + reg[0x0];
    PC = addr - 2;
}

// subroutines

void call(uint16_t opcode)
{
    // store actual address into stack to return to 
    // it after
    stack[SP] = PC;
    ++SP;

    PC = (0x0FFF & opcode) - 2;
}

void ret(uint16_t opcode)
{

    SP--;
    PC = stack[SP];
}
// TODO: implement the 0NNN instruction if needed

// conditional branching using skips


void se(uint16_t opcode)
{   
    uint8_t vx = offset2(opcode);

    uint8_t nn = 0x00FF & opcode;

    // skips next instruction if register vx is equal nn
    if (reg[vx] == nn) {
        PC += 2;
    }
}

void svxevy(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);


    if (reg[vx] == reg[vy]) 
    {
        PC += 2;
    }
}

void sne(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t nn = opcode & 0x00FF;

    if (reg[vx] != nn) 
    {
        PC += 2;
    }
}

void next_if_vx_not_vy(uint16_t opcode)
{ 
    uint8_t vx = offset2(opcode);
    uint8_t vy = offset3(opcode);

    if (reg[vx] != reg[vy])
    {
        PC += 2;
    }
}

// Timers

void set_dt(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);

    DT = reg[vx];
}

void vx_to_dt(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);

    reg[vx] = DT;
}

void set_st(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);

    ST = reg[vx];
}

// TODO: Keypad Input


void vx_to_key(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    reg[vx] = waitkey();
}

void skipifdown(uint16_t opcode) 
{
    uint8_t vx = reg[offset2(opcode)]; // value stored in register vx
    uint8_t is_pressed = keys[vx];     // 1 if key in vx is pressed, 0 otherwise

    // skip if key is pressed
    if (is_pressed) 
    {
        PC += 2;
    }
}

void skipnotdown(uint16_t opcode)
{
    uint8_t vx = reg[offset2(opcode)]; // value stored in register vx
    uint8_t is_pressed = keys[vx];     // 1 if key in vx is pressed, 0 otherwise

    // skip instruction if key is not pressed
    if (!is_pressed) 
    {
        PC += 2;
    }
}


// The I Register
void itoa(uint16_t opcode)
{
    uint16_t nnn = opcode & 0x0FFF;

    I = nnn;
}

void iaddvx(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    I = I + reg[vx];

}

// drawing sprites to the screen

void draw(uint16_t opcode)
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

void cls(uint16_t opcode)
{
    uint16_t row, column;
    for (column = 0; column < GFX_COLUMNS; ++column)
    {

        memset(gfx[column], 0, GFX_ROWS);
    }
    draw_flag = 1;
}


// drawing fonts

void load_char_addr(uint16_t opcode)
{
    // this is the hexadecimal character
    uint8_t hex = reg[offset2(opcode)];

    // set I to the starting address of the data that composes the
    // hexadecimal digit in memory
    I = hex * 5;
}


// Binary-Coded Decimal

void set_BCD(uint16_t opcode)
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

void reg_dump(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t index;

    for (index = 0x0; index <= vx; ++index)
    {
        ram[index + I] = reg[index];
    }
    //I = I + vx + 1;
}

void reg_load(uint16_t opcode)
{
    uint8_t vx = offset2(opcode);
    uint8_t index;

    for (index = 0x0; index <= vx; ++index)
    {
        reg[index] = ram[I+index];
    }
    //I = I + vx + 1;
}
