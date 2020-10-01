#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

// get value of the rflags
extern uint8_t flags(void);

int main()
{
    int i[1];
    uint8_t y = 200;

    getrandom(&i[0], 1, 0x0);
    
    srandom(i[0]);
    long foda_se = rand();
    printf("%x", i[0]);
}