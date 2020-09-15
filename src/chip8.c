#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opcodes.h"
#include "chip8.h"

// program counter
uint16_t PC;

// stack pointer
uint16_t SP;

// stack levels
uint16_t stack[STACK_SIZE];

// store the opcode to be executed
uint16_t opcode;

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


int main(int argc, char *argv[])
{
    // initialize interpreter and load game into memory
    if (argc == 2)
    {
        FILE *filep = fopen(argv[argc-1], "rb");
        initialize ();
        load_game(filep);
    }
    else 
    {
        fprintf(stderr, "usage: ./chip8 <game>\n");
        exit(1);
    }

    game_loop();
}

void initialize()
{

    PC = START_ADDRS;
    opcode = 0;
    SP = 0;
    I = 0;

    // clear
    memset(&gfx, 0, sizeof(gfx));
    memset(&stack, 0, sizeof(stack) / sizeof(stack[0]));
    memset(&reg, 0, sizeof(reg));
    memset(&ram, 0, sizeof(ram));
    
    // load fontset
    int address = FONTSET_ADDRESS;
    for (; address <= sizeof(fonts); address++) {
        ram[address] = fonts[address];
    }



}

void game_loop()
{

    for (; PC <= MAX_RAM; PC += 2) {
        // xor 2 subsequent memory locations to get a 2 bytes opcode
        opcode = (ram[PC] << 8) | ram[PC + 1];
        uint16_t msb = opcode & 0xF000;
        switch (msb)
        {
            case 0x0000 :
                break;
            default:
            
                fprintf(stderr, "[WARNING] uknown opcode %04X", opcode);
        }

        printf("%X %X\n",opcode, PC);
    }
}


void load_game(FILE *filep)
{

    // check for errors 
    int stream_status = ferror(filep);
    if (stream_status != 0) {
        fprintf(stderr, "[ERROR] Error reading file, exiting");
        exit(stream_status);
    }

    // memory address being acessed
    int address = START_ADDRS;

    // read and write one byte at time
    for (address = START_ADDRS; feof(filep) == 0; address++) {
        // check for errors while reading file
        if (ferror(filep) != 0) 
        {
            fprintf(stderr, "[ERROR] Error reading file, exiting");
            exit(ferror(filep));
        } 
        else 
        {
            // load data into memory
            fread(&ram[address], 1, 1, filep);
        }
    }
    fclose(filep);
    fprintf(stdout, "[OK] successfully loaded file into memory\n");
}

void unknown_opcode(uint16_t opcode) 
{
    fprintf(stderr, "[WARNING] uknown opcode %04X", opcode);
}
