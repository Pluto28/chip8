#include <stdint.h>
#include <stdio.h>

// get value of the rflags
extern uint8_t flags(void);

int main()
{
    uint8_t i = 240;
    uint8_t y = 200;

    i = i + 2;
    uint8_t data = (flags());
    printf("%x %x", data, i);
}