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
    //TODO: check if this is behaving the expected way

    // store actual address into stack to return to 
    // it after
    ++SP;
    stack[SP] = PC;

    PC = (0x0FFF & opcode) - 2;
}

void ret(uint16_t opcode)
{
    //TODO: check if this is behaving the expected way
    PC = stack[SP];
    --SP;
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
    uint16_t column;
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
